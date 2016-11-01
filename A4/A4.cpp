#include <glm/ext.hpp>

#include "A4.hpp"
#include "Primitive.hpp"
#include "PhongMaterial.hpp"

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
	std:: cout <<")" << std::endl;

	size_t h = image.height();
	size_t w = image.width();

	glm::vec3 m_view = glm::normalize(view);
	glm::vec3 m_up = glm::normalize(up );
	glm::vec3 m_side = glm::normalize(glm::cross(m_view, m_up));

	int progress = 0;
	std::cout << "Progress : " << progress << "%" << std::endl;
	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {

			glm::vec3 ray = m_view + (-1 + (x + 0.5) / double(w) * 2) * tan(glm::radians(fovy / 2)) * double(w) / double(h) * m_side
								+ (-1 + (y + 0.5) / double(h) * 2) * tan(glm::radians(fovy / 2)) * -m_up;
			ray = glm::normalize(ray);

			//std::cout << "eye " << glm::to_string(eye) << " ray " << glm::to_string(ray) << std::endl;
			Intersection *intersection = root->intersect(glm::vec4(eye, 1), glm::vec4(ray, 0));

			if (intersection != NULL && intersection->hit) {
				// Ambient Reflection
				PhongMaterial *material = (PhongMaterial*)intersection->material;
				image(x, y, 0) = ambient.x * material->getkd().x;
				image(x, y, 1) = ambient.y * material->getkd().y;
				image(x, y, 2) = ambient.z * material->getkd().z;

				for(const Light * light : lights) {
					//std::cout << "\t\t" <<  *light << std::endl;
					float r = glm::length(light->position - intersection->point);
					float falloff = light->falloff[0] + light->falloff[1] * r + light->falloff[2] * r * r;
					glm::vec3 normal = glm::normalize(intersection->normal);
					glm::vec3 l = glm::normalize(light->position - intersection->point);
					if (glm::dot(normal, l) > 0) { 
						image(x, y, 0) += light->colour.x / falloff * material->getkd().x * glm::dot(normal, l);
						image(x, y, 1) += light->colour.y / falloff * material->getkd().y * glm::dot(normal, l);
						image(x, y, 2) += light->colour.z / falloff * material->getkd().z * glm::dot(normal, l);

						glm::vec3 reflection = glm::normalize(2.0f * glm::dot(normal - l, normal) - l);
						glm::vec3 v = glm::normalize(eye - intersection->point);
						image(x, y, 0) += light->colour.x / falloff * material->getks().x * glm::dot(reflection, v);
						image(x, y, 1) += light->colour.y / falloff * material->getks().y * glm::dot(reflection, v);
						image(x, y, 2) += light->colour.z / falloff * material->getks().z * glm::dot(reflection, v);
					}
				}
			} else {
				// Red: increasing from top to bottom
				image(x, y, 0) = (double)y / h;
				// Green: increasing from left to right
				image(x, y, 1) = (double)x / w;
				// Blue: in lower-left and upper-right corners
				image(x, y, 2) = ((y < h/2 && x < w/2)
							  || (y >= h/2 && x >= w/2)) ? 1.0 : 0.0;
			}

			if (((y+1) * w + (x+1)) / (h * w) * 100 >= (progress + 10)) {
				progress += 10;
				std::cout << "Progress : " << progress << "%" << std::endl;
			}
		}
	}

}
