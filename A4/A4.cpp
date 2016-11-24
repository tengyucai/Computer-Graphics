#include <glm/ext.hpp>

#include <lodepng/lodepng.h>

#include "A4.hpp"
#include "Primitive.hpp"
#include "PhongMaterial.hpp"
#include <tuple>

using namespace std;
using namespace glm;

#define kEpsilon 0.001
#define SAMPLE_SIZE 3

glm::vec3 getLights(
	SceneNode * root,
	const std::list<Light *> & lights,
	const glm::vec3 & ambient,
	glm::vec3 eye,
	glm::vec3 view,
	// Intersection *intersection,
	int depth);

tuple<bool, double, vec3> getRefractionRay(vec3 dir, vec3 normal, double ni);

void A4_Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

  // Fill in raytracing code here...

  std::cout << "Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std::cout <<")" << std::endl;

	std::vector<unsigned char> background;
	unsigned bg_width, bg_height;
	unsigned bg_error = lodepng::decode(background, bg_width, bg_height, "Assets/background.png");
	if (bg_error) {
		std::cout << "Decoder error " << bg_error << ": " << lodepng_error_text(bg_error) << std::endl;
	} else {
		std::cout << "Load background successed" << std::endl;
	}

	size_t h = image.height();
	size_t w = image.width();

	glm::vec3 m_view = glm::normalize(view);
	glm::vec3 m_up = glm::normalize(up);
	glm::vec3 m_side = glm::normalize(glm::cross(m_view, m_up));

	int progress = 0;
	std::cout << "Progress : " << progress << " %" << std::endl;
	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {

			glm::vec3 c = glm::vec3(0, 0, 0);
			for (int i = 0; i < SAMPLE_SIZE; ++i) {
				for (int j = 0; j < SAMPLE_SIZE; ++j) {
					glm::vec3 ray = m_view + (-1 + (x + 0.2 + i*0.3) / double(w) * 2) * tan(glm::radians(fovy / 2)) * double(w) / double(h) * m_side
										+ (-1 + (y + 0.2 + j*0.3) / double(h) * 2) * tan(glm::radians(fovy / 2)) * -m_up;
					ray = glm::normalize(ray);

					c += getLights(root, lights, ambient, eye, ray, 4);

					// Intersection *intersection = root->intersect(glm::vec4(eye, 1), glm::vec4(ray, 0));

					// if (intersection != NULL && intersection->hit) {

					// 	glm::vec3 colour = getLights(root, lights, ambient, eye, -1.0*ray, intersection, 3);

					// 	c += colour;
					// } else {
					// 	// int bg_x = (float)bg_width / w * x;
					// 	// int bg_y = (float)bg_height / h * y;

					// 	// c.x += (float)background[4*(bg_y * bg_width + bg_x)] / 255;
					// 	// c.y += (float)background[4*(bg_y * bg_width + bg_x) + 1] / 255;
					// 	// c.z += (float)background[4*(bg_y * bg_width + bg_x) + 2] / 255;
					// 	c.x = 0;
					// 	c.y = 0;
					// 	c.z = 0;
					// }

					// delete intersection;
				}
			}
			image(x, y, 0) = c.x / (SAMPLE_SIZE * SAMPLE_SIZE);
			image(x, y, 1) = c.y / (SAMPLE_SIZE * SAMPLE_SIZE);
			image(x, y, 2) = c.z / (SAMPLE_SIZE * SAMPLE_SIZE);
//}
			if (double(y * w + x + 1) / (h * w) * 100 >= (progress + 5)) {
				progress += 5;
				std::cout << "Progress : " << progress << " %" << std::endl;
			}
		}
	}

}

glm::vec3 getLights(
	SceneNode * root,
	const std::list<Light *> & lights,
	const glm::vec3 & ambient,
	glm::vec3 eye,
	glm::vec3 dir,
	// Intersection *intersection,
	int depth) {

	float r = 0.0f, g = 0.0f, b = 0.0f;

	eye = eye + kEpsilon * dir;
	Intersection *intersection = root->intersect(glm::vec4(eye, 1), glm::vec4(dir, 0));

	if (intersection != NULL && intersection->hit) {
	
		PhongMaterial *material = (PhongMaterial*)intersection->material;

		r += ambient.x * material->getkd().x;
		g += ambient.y * material->getkd().y;
		b += ambient.z * material->getkd().z;
		for (const Light * light : lights) {
			float distance = glm::length(light->position - intersection->point);
			float falloff = light->falloff[0] + light->falloff[1] * distance + light->falloff[2] * distance * distance;
			glm::vec3 intensity = light->colour / falloff;

			glm::vec3 normal = glm::normalize(intersection->normal);
			glm::vec3 l = glm::normalize(light->position - intersection->point);

			if (glm::dot(normal, l) > -kEpsilon) {
				vec3 p = intersection->point + kEpsilon * l;
				Intersection *s_intersection = root->intersect(glm::vec4(p, 1), glm::vec4(l, 0));

				if (s_intersection == NULL || !s_intersection->hit ||
					glm::dot(intersection->point - s_intersection->point, s_intersection->normal) < 0) {
					glm::vec3 diffusion = intensity * material->getkd() * glm::dot(normal, l);

					glm::vec3 reflection = 2.0f * glm::dot(normal, l) * normal - l;
					glm::vec3 v = glm::normalize(eye - intersection->point);
					glm::vec3 specular = intensity * material->getks() * pow(glm::dot(reflection, v), material->getShininess());

					r += diffusion.x + specular.x;
					g += diffusion.y + specular.y;
					b += diffusion.z + specular.z;
				}

				delete s_intersection;
			}
		}

		if (--depth > 0) {
			double R = 1.0;
			if (material->getShininess() > 0) {
				glm::vec3 normal = glm::normalize(intersection->normal);
				glm::vec3 reflection_view = glm::normalize(2 * glm::dot(-1.0 * dir, normal) * normal - -1.0 * dir);
				eye = intersection->point + kEpsilon * reflection_view;
				// Intersection *new_intersection = root->intersect(glm::vec4(eye, 1), glm::vec4(reflection_view, 0));

				// if (new_intersection->hit) {
				glm::vec3 reflection = getLights(root, lights, ambient, eye, reflection_view, depth);
				if(material->getShininess() >= 128){// purly reflective
					r = r / 128 + reflection.x;
					g = g / 128 + reflection.y;
					b = b / 128 + reflection.z;
				} else {
					r = r * ((128 - material->getShininess()) / 128) + reflection.x * (material->getShininess() / 128);
					g = g * ((128 - material->getShininess()) / 128) + reflection.y * (material->getShininess() / 128);
					b = b * ((128 - material->getShininess()) / 128) + reflection.z * (material->getShininess() / 128);
				}

				// delete new_intersection;
			}

			if (material->isTransparent()) {
				glm::vec3 normal = glm::normalize(intersection->normal);
				tuple<bool, double, vec3> ret = getRefractionRay(dir, normal, material->getRefractionIndex());
				bool total_internal_reflection = std::get<0>(ret);
				R = std::get<1>(ret);
	      		if (!total_internal_reflection) {
					vec3 refracted_ray = std::get<2>(ret);
					eye = intersection->point + kEpsilon * refracted_ray;
					vec3 refraction = getLights(root, lights, ambient, eye, refracted_ray, depth);
					r += refraction.x * material->getks().x;
					g += refraction.y * material->getks().y;
					b += refraction.z * material->getks().z;
				}
			}
		}
	}
	delete intersection;
	return glm::vec3(r, g, b);
}

tuple<bool, double, vec3> getRefractionRay(vec3 dir, vec3 normal, double ni) {
	glm::vec3 new_dir;
	double cosI = dot(dir, normal);
	double n1 = 1.0, n2;
	double R = 1.0;

	if (cosI > 0) { // direction and normal are in same direction, ray from inside primitive
		n1 = ni;
		n2 = 1.0;
		normal = -1 * normal;
	} else { // ray from outside of primitive
		n1 = 1.0;
		n2 = ni;
		cosI = -cosI;
	}

	// check total internal reflection
	double nr = n1 / n2;
	double cosT = 1.0 - (nr * nr) * (1.0 - (cosI * cosI));
	if (cosT >= 0) {
		cosT = sqrt(cosT);

		// compute Fresnel coefficient
		double a = (n1 * cosI - n2 * cosT) / (n1 * cosI + n2 * cosT);
		double b = (n2 * cosI - n1 * cosT) / (n2 * cosI + n1 * cosT);
		R = ((a * a) + (b * b)) * 0.5;

		// cast the refracted ray
		new_dir = glm::vec3(nr * dir + (nr * cosI - cosT) * normal);
	}

	return tuple<bool, double, vec3>(cosT < 0, R, new_dir);
} 
