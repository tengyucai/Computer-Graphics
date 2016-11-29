#include <glm/ext.hpp>

#include <lodepng/lodepng.h>
#include <math.h>
#include <tuple>
#include <thread>
#include <mutex>

#include "A5.hpp"
#include "Primitive.hpp"
#include "PhongMaterial.hpp"
#include "PhotonMap.hpp"

using namespace std;
using namespace glm;

#define kEpsilon 0.001
#define SAMPLE_SIZE 1
#define NUM_THREADS_PHOTON 4
#define NUM_THREADS_RENDER 1

mutex pm_mtx;
mutex pc_mtx;
mutex image_mtx;
mutex ic_mtx;
mutex progress_mtx;

glm::vec3 getLights(
	SceneNode * root,
	const std::list<Light *> & lights,
	const glm::vec3 & ambient,
	glm::vec3 eye,
	glm::vec3 view,
	Intersection *intersection,
	int depth);

vec3 getPerfectReflectionRay(vec3 dir, vec3 normal);
vec3 getRandomDiffuseReflectionRay(vec3 normal);
tuple<bool, double, vec3> getRefractionRay(vec3 dir, vec3 normal, double ni, bool inside);

int tracePhoton(SceneNode *root, vec3 orig, vec3 dir, Photon_map *photon_map, vec3 power, int depth);
bool traceScene(SceneNode *root, vec3 orig, vec3 ray, Photon_map *photon_map, vec3 &colour, int depth);

void photonTracing(
	SceneNode *root, 
	Photon_map *photon_map, 
	const list<Light *> &lights
);

void renderUsingPhotonMap(
	SceneNode *root,
	const glm::vec3 & eye,
	const glm::vec3 & view,
	const glm::vec3 & up,
	double fovy,
	const glm::vec3 & ambient,
	const std::list<Light *> & lights,
	Photon_map *photon_map, 
	Image &image,
	int thread_num
);

void photonMapping(
	SceneNode * root,
	Image & image,
	// Viewing parameters
	const glm::vec3 & eye,
	const glm::vec3 & view,
	const glm::vec3 & up,
	double fovy,
	// Lighting parameters
	const glm::vec3 & ambient,
	const std::list<Light *> & lights,
	Photon_map *photon_map
);


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

	// { // ray tracing
	// 	size_t h = image.height();
	// 	size_t w = image.width();

	// 	glm::vec3 m_view = glm::normalize(view);
	// 	glm::vec3 m_up = glm::normalize(up);
	// 	glm::vec3 m_side = glm::normalize(glm::cross(m_view, m_up));

	// 	int progress = 0;
	// 	std::cout << "Progress : " << progress << " %" << std::endl;
	// 	for (uint y = 0; y < h; ++y) {
	// 		for (uint x = 0; x < w; ++x) {

	// 			glm::vec3 c = glm::vec3(0, 0, 0);
	// 			for (int i = 0; i < SAMPLE_SIZE; ++i) {
	// 				for (int j = 0; j < SAMPLE_SIZE; ++j) {
	// 					glm::vec3 ray = m_view + (-1 + (x + 0.2 + i*0.3) / double(w) * 2) * tan(glm::radians(fovy / 2)) * double(w) / double(h) * m_side
	// 										+ (-1 + (y + 0.2 + j*0.3) / double(h) * 2) * tan(glm::radians(fovy / 2)) * -m_up;
	// 					ray = glm::normalize(ray);

	// 					Intersection *intersection = root->intersect(glm::vec4(eye, 1), glm::vec4(ray, 0));

	// 					if (intersection != NULL && intersection->hit) {

	// 						glm::vec3 colour = getLights(root, lights, ambient, eye, -1.0*ray, intersection, 3);

	// 						c += colour;
	// 					}

	// 					delete intersection;
	// 				}
	// 			}
	// 			image(x, y, 0) = c.x / (SAMPLE_SIZE * SAMPLE_SIZE);
	// 			image(x, y, 1) = c.y / (SAMPLE_SIZE * SAMPLE_SIZE);
	// 			image(x, y, 2) = c.z / (SAMPLE_SIZE * SAMPLE_SIZE);

	// 			if (double(y * w + x + 1) / (h * w) * 100 >= (progress + 5)) {
	// 				progress += 5;
	// 				std::cout << "Progress : " << progress << " %" << std::endl;
	// 			}
	// 		}
	// 	}
	// }

	// Photon mapping
	Photon_map *m_photon_map = new Photon_map(200000000);
	photonMapping(root, image, eye, view, up, fovy, ambient, lights, m_photon_map);
	// vec3 normal = vec3(0, 1, 0);
	// vec3 dir = vec3(2, -1, 0);
	// for (int i = 0; i < 1000; ++i) {
	// 	vec3 refraction = getRandomDiffuseReflectionRay(normal);
	// 	cout << to_string(refraction) << endl;
	// 	image((refraction.x + 1)*128, (refraction.z+1)*128, 0) = 1;
	// }
}

static int emited_photons = 0;
static int num_photons = 0;
static double progress = 0;
static int image_count = 0;

void photonMapping(
	SceneNode *root,
	Image & image,
	// Viewing parameters
	const glm::vec3 & eye,
	const glm::vec3 & view,
	const glm::vec3 & up,
	double fovy,
	// Lighting parameters
	const glm::vec3 & ambient,
	const std::list<Light *> & lights,
	Photon_map *photon_map
) {
	thread threads_photon[NUM_THREADS_PHOTON];
	thread threads_render[NUM_THREADS_RENDER];

	cout << "Building photon map" << endl;
	for (int i = 0; i < NUM_THREADS_PHOTON; ++i) {
		threads_photon[i] = thread(photonTracing, root, photon_map, ref(lights));
	}
	for (int i = 0; i < NUM_THREADS_PHOTON; ++i) {
		threads_photon[i].join();
	}
	cout << "Scaling photon power" << endl;
	photon_map->scale_photon_power(double(1) / emited_photons);
	
	cout << "Balancing photon map" << endl;
	photon_map->balance();
	cout << "Photon map done, num_photons : " << num_photons << " emited_photons : " << emited_photons << endl;

	// photonTracing(root, photon_map, lights);
	cout << "Start rendering" << endl;
	cout << "Progress : 0 %" << endl;
	progress = 0;
	for (int i = 0; i < NUM_THREADS_RENDER; ++i) {
		threads_render[i] = thread(renderUsingPhotonMap, root, ref(eye), ref(view), ref(up), fovy, ref(ambient), ref(lights), photon_map, ref(image), i);
	}
	for (int i = 0; i < NUM_THREADS_RENDER; ++i) {
		threads_render[i].join();
	}
	cout << "Done rendering!" << endl;
	// renderUsingPhotonMap(root, eye, view, up, fovy, ambient, lights, photon_map, image);
}

// Assume single point light source for now
void photonTracing(SceneNode *root, Photon_map *photon_map, const list<Light *> &lights) {
	vec3 orig;
	vec3 dir;
	int num_photons_per_light = 1000000;
	
	for (Light *light : lights) {
		double x, y, z;
		num_photons = 0;
		while (num_photons < num_photons_per_light) {
			// Origin
			double origin_x = (((double) rand() / (RAND_MAX)) - 0.5) * 0.1;
			double origin_z = (((double) rand() / (RAND_MAX)) - 0.5) * 0.1;
			orig = light->position + vec3(origin_x, -kEpsilon, origin_z);
			// Direction
			do {
				x = ((double) rand() / (RAND_MAX)) * 2 - 1;
				y = ((double) rand() / (RAND_MAX)) * 2 - 1;
				z = ((double) rand() / (RAND_MAX)) * 2 - 1;
			} while (x*x + y*y + z*z > 1.0);
			dir = normalize(vec3(x, y, z));
			// cout << to_string(dir) << endl;
			vec3 power = glm::vec3(4, 4, 4);//glm::vec3(1000000, 1000000, 1000000);// light->power * light->colour;
			int photons = tracePhoton(root, orig, dir, photon_map, power, 0);
			pc_mtx.lock();
				num_photons += photons;
			pc_mtx.unlock();
			pc_mtx.lock();
				++emited_photons;
			pc_mtx.unlock();
			if (double(num_photons) / num_photons_per_light * 100 >= (progress + 0.1)) {
				progress_mtx.lock();
					progress += 0.1;
				progress_mtx.unlock();
				cout << "Progress : " << progress << " %" << endl;
			}
		}
	}
}

int tracePhoton(SceneNode *root, vec3 orig, vec3 dir, Photon_map *photon_map, vec3 power, int depth) {
	if (++depth > 10) return 0;

	orig = orig + kEpsilon * dir;
	Intersection *intersection = root->intersect(glm::vec4(orig, 1), glm::vec4(dir, 0));
	if (!intersection->hit) {
		delete intersection;
		return 0;
	}
	PhongMaterial *material = (PhongMaterial *)intersection->material;
	vec3 kd = material->getkd();
	vec3 ks = material->getks();
	double avg_kd = (kd.x + kd.y + kd.z) / 3;  			// [0, 1] 0.7
	double avg_ks = (ks.x + ks.y + ks.z) / 3;			// [0, 1] 0.7
	double transparency = material->getTransparency();
	double absorption = 0.01;
	avg_kd = avg_kd / (avg_kd + avg_ks + absorption) * (1 - transparency);
	avg_ks = avg_ks / (avg_kd + avg_ks + absorption) * (1 - transparency);
	absorption = absorption / (avg_kd + avg_ks + absorption) * (1 - transparency);
	double ksi = ((double) rand() / (RAND_MAX));

	// 1. diffuse reflection
	if (ksi <= avg_kd) {
		// cout << "diffuse" << endl;
		int photon_added = 0;
		vec3 new_power = vec3(power.x*kd.x/avg_kd, power.y*kd.y/avg_kd, power.z*kd.z/avg_kd);
		float pow[3] = { new_power.x, new_power.y, new_power.z };
		float pos[3] = { intersection->point.x, intersection->point.y, intersection->point.z};
		float d[3] = {dir.x, dir.y, dir.z};
		pm_mtx.lock();
			photon_map->store(pow, pos, d);
		pm_mtx.unlock();
		// cout << to_string(intersection->point) << " " << to_string(intersection->normal) << endl;
		++photon_added;
		vec3 new_dir = getRandomDiffuseReflectionRay(intersection->normal);
		vec3 point = intersection->point;
		delete intersection;
		return photon_added + tracePhoton(root, point, new_dir, photon_map, new_power, depth);
	} 
	// 2. specular reflection
	else if (avg_kd < ksi && ksi <= (avg_kd + avg_ks)) {
		// cout << "specular" << endl;
		// need to scale the power of the reflected photon
		vec3 new_power = vec3(power.x*ks.x/avg_ks, power.y*ks.y/avg_ks, power.z*ks.z/avg_ks);
		vec3 new_dir = getPerfectReflectionRay(dir, intersection->normal);
		vec3 point = intersection->point;
		delete intersection;
		return tracePhoton(root, point, new_dir, photon_map, new_power, depth);
	}
	// 3. refraction
	else if ((avg_kd + avg_ks) < ksi && ksi <= (avg_kd + avg_ks + transparency)) {
		// cout << "refraction" << endl;
		tuple<bool, double, vec3> result = getRefractionRay(dir, intersection->normal, material->getRefractionIndex(), intersection->inside);
		bool total_internal_reflection = get<0>(result);
		if (!total_internal_reflection) {
			vec3 new_dir = get<2>(result);
			vec3 point = intersection->point;
			delete intersection;
			// cout << to_string(new_dir) << endl;
			// cout << "refraction" << endl;
			return tracePhoton(root, point, new_dir, photon_map, power, depth);
		} else {
			cout << "total_internal_reflection" << endl;
		}
	}
	// 4. absorption
	else if ((avg_kd + avg_ks + transparency) < ksi && ksi <= 1) {
		// cout << "absorption" << endl;
		// Do nothing.
		delete intersection;
		return 0;
	}
}

void renderUsingPhotonMap(
	SceneNode *root,
	const glm::vec3 & eye,
	const glm::vec3 & view,
	const glm::vec3 & up,
	double fovy,
	const glm::vec3 & ambient,
	const std::list<Light *> & lights,
	Photon_map *photon_map, 
	Image &image,
	int thread_num
) {
	int h = image.height();
	int w = image.width();

	vec3 m_view = normalize(view);
	vec3 m_up = normalize(up);
	vec3 m_side = normalize(cross(m_view, m_up));

	int size_per_thread = h / NUM_THREADS_RENDER;

	for (uint image_y = thread_num * size_per_thread; image_y < (thread_num+1)*size_per_thread && image_y < h; ++image_y) {
		for (uint image_x = 0; image_x < w; ++image_x) {

			vec3 colour = vec3(0, 0, 0);
			vec3 ray = m_view + (-1 + (image_x + 0.5f) / double(w) * 2) * tan(glm::radians(fovy / 2)) * double(w) / double(h) * m_side
							+ (-1 + (image_y + 0.5f) / double(h) * 2) * tan(glm::radians(fovy / 2)) * -m_up;
			ray = glm::normalize(ray);

			// Intersection *intersection = root->intersect(glm::vec4(eye, 1), glm::vec4(ray, 0));
			// if (intersection != NULL && intersection->hit) {
			// 	// colour = getLights(root, lights, ambient, eye, -1.0*ray, intersection, 3);
			// 	float irrad[3] = { 0, 0, 0 };
			// 	float pos[3] = { intersection->point.x, intersection->point.y, intersection->point.z };
			// 	float normal[3] = { intersection->normal.x, intersection->normal.y, intersection->normal.z };
			// 	photon_map->irradiance_estimate(irrad, pos, normal, 0.008f, 10);
			// 	cout << irrad[0] << " " << irrad[1] << " " << irrad[2] << endl;
			// 	colour += vec3(irrad[0], irrad[1], irrad[2]);
			// 	image(x, y, 0) = colour.x;
			// 	image(x, y, 1) = colour.y;
			// 	image(x, y, 2) = colour.z;
			// }
			// delete intersection;

			if (traceScene(root, eye, ray, photon_map, colour, 0)) {
				// cout << to_string(colour) << endl;
				// cout << colour.x << " " << colour.y << " " << colour.z << endl;
				image_mtx.lock();
					image(image_x, image_y, 0) = colour.x;
					image(image_x, image_y, 1) = colour.y;
					image(image_x, image_y, 2) = colour.z;
				image_mtx.unlock();
			} else {
				image_mtx.lock();
					image(image_x, image_y, 0) = 0;
					image(image_x, image_y, 1) = 0;
					image(image_x, image_y, 2) = float(image_x) / h;
				image_mtx.unlock();
			}

			ic_mtx.lock();
				++image_count;
			ic_mtx.unlock();
			cout << "Progress : " << double(image_count) / (h * w) * 100 << " %" << endl;

			// if (double(image_count) / (h * w) * 100 >= (progress + 1)) {
			// 	progress_mtx.lock();
			// 		progress += 1;
			// 		cout << "Progress : " << progress << " %" << endl;
			// 	progress_mtx.unlock();
			// }
		}
	}
}

bool traceScene(SceneNode *root, vec3 orig, vec3 dir, Photon_map *photon_map, vec3 &colour, int depth) {
	if (++depth > 10) return false;

	orig = orig + kEpsilon * dir;
	Intersection *intersection = root->intersect(glm::vec4(orig, 1), glm::vec4(dir, 0));

	if (intersection->hit) {
		PhongMaterial *material = (PhongMaterial *)intersection->material;
		
		if (material->isDiffuse()) {
			// cout << "diffuse" << endl;
			float irrad[3] = { 0, 0, 0 };
			float pos[3] = { intersection->point.x, intersection->point.y, intersection->point.z };
			float normal[3] = { intersection->normal.x, intersection->normal.y, intersection->normal.z };
			pm_mtx.lock();
				photon_map->irradiance_estimate(irrad, pos, normal, 0.2f, 5000);
			pm_mtx.unlock();
			// cout << irrad[0] << " " << irrad[1] << " " << irrad[2] << endl;
			colour += vec3(irrad[0], irrad[1], irrad[2]) * (1 - material->getTransparency());
		}

		if (material->isSpecular()) {
			// cout << "Specular" << endl;
			vec3 reflect_colour;
			vec3 reflect_ray = getPerfectReflectionRay(dir, intersection->normal);
			// cout << to_string(intersection->normal) << endl;
			// cout << to_string(dir) << " " << to_string(reflect_ray) << endl;
			vec3 point = intersection->point;// + kEpsilon * intersection->normal;
			if (traceScene(root, point, reflect_ray, photon_map, reflect_colour, depth)) {
				colour += reflect_colour * (1 - material->getTransparency());
			} 
		}

		if (material->isTransparent()) {
			tuple<bool, double, vec3> result = getRefractionRay(dir, intersection->normal, material->getRefractionIndex(), intersection->inside);
			bool total_internal_reflection = get<0>(result);
			if (!total_internal_reflection) {
				// cout << "refraction" << endl;
				vec3 new_dir = get<2>(result);
				vec3 refract_colour;
				vec3 point = intersection->point;// - 0.0001 * intersection->normal;
				if (traceScene(root, point, new_dir, photon_map, refract_colour, depth)) {
					colour += refract_colour;// * material->getTransparency();
				} 
			}
		}

		return true;
	}
	delete intersection;

	return false;
}

vec3 getPerfectReflectionRay(vec3 dir, vec3 normal) {
	normal = normalize(normal);
	vec3 refl = 2.0f * glm::dot(-dir, normal) * normal - -dir;
	return normalize(refl);
}

vec3 getRandomDiffuseReflectionRay(vec3 normal) {
	vec3 dir;
	double rand1 = ((double) rand() / (RAND_MAX)); // [0, 1]
	double rand2 = ((double) rand() / (RAND_MAX)); // [0, 1]
	double cos_theta = sqrt(rand1);
	double phi = 2 * M_PI * rand2;
	vec3 w = normal;
	vec3 u = fabs(w.x) > 0.1 ? cross(vec3(0, 1, 0), w) : cross(vec3(1, 0, 0), w);
	u = normalize(u);
	vec3 v = cross(w, u);
	dir = cos(phi) * cos_theta * u + sin(phi) * cos_theta * v + sqrt(1-rand1) * w;
	return normalize(dir);
}

tuple<bool, double, vec3> getRefractionRay(vec3 dir, vec3 normal, double ni, bool inside) {
	glm::vec3 new_dir;
	double cosI = dot(dir, normal);
	double n1, n2;
	double R = 1.0;

	// if (cosI > 0) { // direction and normal are in same direction, ray from inside primitive
	// 	n1 = ni;
	// 	n2 = 1.0;
	// 	normal = -1 * normal;
	// } else { // ray from outside of primitive
	// 	n1 = 1.0;
	// 	n2 = ni;
	// 	cosI = -cosI;
	// }
	if (inside) {
		n1 = ni;
		n2 = 1.0;
	} else {
		n1 = 1.0;
		n2 = ni;
	}
	cosI = fabs(cosI);

	// check total internal reflection
	double nr = n1 / n2;
	double cosT = 1.0 - (nr * nr) * (1.0 - (cosI * cosI));

	// cout << to_string(dir) << " " << to_string(normal) <<  " n1 " << n1 << " n2 " << n2 << endl;
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
