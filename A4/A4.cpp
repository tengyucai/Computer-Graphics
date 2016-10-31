#include <glm/ext.hpp>

#include "A4.hpp"

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
	glm::vec3 m_up = glm::normalize(up);
	glm::vec3 m_side = glm::normalize(glm::cross(m_view, m_up));

	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {

			glm::vec3 ray = m_view + (-1 + (x + 0.5) / double(w) * 2) * tan(glm::radians(fovy / 2)) * double(w) / double(h) * m_side
								+ (-1 + (y + 0.5) / double(h) * 2) * tan(glm::radians(fovy / 2)) * -m_up;
			ray = glm::normalize(ray);

			//std::cout << "eye " << glm::to_string(eye) << " ray " << glm::to_string(ray) << std::endl;
			bool intersect = root->intersect(glm::vec4(eye, 1), glm::vec4(ray, 0));

			if (intersect) {
				image(x, y, 0) = 0;
				image(x, y, 1) = 0;
				image(x, y, 2) = 0;
			} else {
				// Red: increasing from top to bottom
				image(x, y, 0) = (double)y / h;
				// Green: increasing from left to right
				image(x, y, 1) = (double)x / w;
				// Blue: in lower-left and upper-right corners
				image(x, y, 2) = ((y < h/2 && x < w/2)
							  || (y >= h/2 && x >= w/2)) ? 1.0 : 0.0;
			}
		}
	}

}
