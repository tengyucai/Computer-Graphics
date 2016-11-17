#include <glm/ext.hpp>

#include <lodepng/lodepng.h>

#include "A5.hpp"
#include "Primitive.hpp"
#include "PhongMaterial.hpp"

#define kEpsilon 0.001
#define SAMPLE_SIZE 1

glm::vec3 getLights(
	SceneNode * root,
	const std::list<Light *> & lights,
	const glm::vec3 & ambient,
	glm::vec3 eye,
	glm::vec3 view,
	Intersection *intersection,
	int depth);

void A5_Render(
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

  std::cout << "Calling A5_Render(\n" <<
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

					Intersection *intersection = root->intersect(glm::vec4(eye, 1), glm::vec4(ray, 0));

					if (intersection != NULL && intersection->hit) {

						glm::vec3 colour = getLights(root, lights, ambient, eye, -1.0*ray, intersection, 3);

						c += colour;
					} else {
						// int bg_x = (float)bg_width / w * x;
						// int bg_y = (float)bg_height / h * y;

						// c.x += (float)background[4*(bg_y * bg_width + bg_x)] / 255;
						// c.y += (float)background[4*(bg_y * bg_width + bg_x) + 1] / 255;
						// c.z += (float)background[4*(bg_y * bg_width + bg_x) + 2] / 255;
					}

					delete intersection;
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
	glm::vec3 view,
	Intersection *intersection,
	int depth) {

	float r = 0.0f, g = 0.0f, b = 0.0f;
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
			Intersection *s_intersection = root->intersect(glm::vec4(intersection->point, 1), glm::vec4(l, 0));

			if (s_intersection == NULL || !s_intersection->hit) {
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
		if (material->getShininess() > 0) {
			glm::vec3 normal = glm::normalize(intersection->normal);
			glm::vec3 reflection_view = glm::normalize(2 * glm::dot(view, normal) * normal - view);
			eye = intersection->point + 0.0001 * reflection_view;
			Intersection *new_intersection = root->intersect(glm::vec4(eye, 1), glm::vec4(reflection_view, 0));

			if (new_intersection->hit) {
				glm::vec3 reflection = getLights(root, lights, ambient, eye, -1.0*reflection_view, new_intersection, --depth);
				if(material->getShininess() >= 128){// purly reflective
					r = r / 128 + reflection.x;
					g = g / 128 + reflection.y;
					b = b / 128 + reflection.z;
				} else {
					r = r * ((128 - material->getShininess()) / 128) + reflection.x * (material->getShininess() / 128);
					g = g * ((128 - material->getShininess()) / 128) + reflection.y * (material->getShininess() / 128);
					b = b * ((128 - material->getShininess()) / 128) + reflection.z * (material->getShininess() / 128);
				}
			}

			delete new_intersection;
		}
	}

	return glm::vec3(r, g, b);
}


// PhongMaterial *material = (PhongMaterial*)intersection->material;
// image(x, y, 0) = ambient.x * material->getkd().x;
// image(x, y, 1) = ambient.y * material->getkd().y;
// image(x, y, 2) = ambient.z * material->getkd().z;

// for(const Light * light : lights) {
// 	float distance = glm::length(light->position - intersection->point);
// 	float falloff = light->falloff[0] + light->falloff[1] * distance + light->falloff[2] * distance * distance;
// 	glm::vec3 intensity = light->colour / falloff;

// 	// if (!inShadow(intersection, light->position, root)) {
// 	glm::vec3 normal = glm::normalize(intersection->normal);
// 	glm::vec3 l = glm::normalize(light->position - intersection->point);

// 	if (glm::dot(normal, l) > -kEpsilon) {
// 		Intersection *s_intersection = root->intersect(glm::vec4(intersection->point, 1), glm::vec4(l, 0));

// 		if (s_intersection == NULL || !s_intersection->hit) {
// 			glm::vec3 diffusion = intensity * material->getkd() * glm::dot(normal, l);

// 			glm::vec3 reflection = 2.0f * glm::dot(normal, l) * normal - l;
// 			glm::vec3 v = glm::normalize(eye - intersection->point);
// 			glm::vec3 specular = intensity * material->getks() * pow(glm::dot(reflection, v), material->getShininess());

// 			image(x, y, 0) += diffusion.x + specular.x;
// 			image(x, y, 1) += diffusion.y + specular.y;
// 			image(x, y, 2) += diffusion.z + specular.z;
// 		}

// 		delete s_intersection;
// 	}
// }
