#include "Primitive.hpp"
#include "polyroots.hpp"

#include <iostream>
#include <glm/ext.hpp>

#define kEpsilon 0.001

Primitive::~Primitive()
{
}

Intersection* Primitive::intersect(const glm::vec3 &eye, const glm::vec3 &ray) {
	return new Intersection();
}

Sphere::~Sphere()
{
	delete n_sphere;
}

Intersection* Sphere::intersect(const glm::vec3 &eye, const glm::vec3 &ray) {
	return n_sphere->intersect(eye, ray);
}

Cube::~Cube()
{
	delete n_box;
}

Intersection* Cube::intersect(const glm::vec3 &eye, const glm::vec3 &ray) {
	return n_box->intersect(eye, ray);
}

NonhierSphere::~NonhierSphere()
{
}

Intersection* NonhierSphere::intersect(const glm::vec3 &eye, const glm::vec3 &ray) {
	// std::cout << "intersect Sphere: " << std::endl;
	// std::cout << "eye " << glm::to_string(eye) << std::endl;
	// std::cout << "ray " << glm::to_string(ray) << std::endl;
	double roots[2];
	double A = glm::dot(ray, ray);
	double B = 2 * glm::dot(eye - m_pos, ray);
	double C = glm::dot(eye - m_pos, eye - m_pos) - m_radius * m_radius;
	size_t num = quadraticRoots(A, B, C, roots);
	// std::cout << "A " << A << " B " << B << " C " << C << std::endl;

	if (num == 0) {
		return new Intersection();
	} else {
		double min = std::min<double>(roots[0], roots[1]);
		double t = (num == 1) ? roots[0] : ((min < 0) ? std::max<double>(roots[0], roots[1]) : min);
		bool inside = num == 2 && roots[0] < 0 && roots[1] >= 0;
		// float t = roots[0];
		// if (num == 2 && roots[1] < roots[0]) t = roots[1];
		if (t < kEpsilon) return new Intersection();
		glm::vec3 point = eye + t * ray;
		glm::vec3 normal = point - m_pos;
		Intersection *intersection = new Intersection(true, t, point, normal);
		intersection->inside = inside;
		return intersection;
	}
}

BoundedBox::~BoundedBox()
{
}

Intersection* BoundedBox::intersect(const glm::vec3 &eye, const glm::vec3 &ray) {
	glm::vec3 bounds[2];
	int sign[3];

	bounds[0] = glm::vec3(xmin, ymin, zmin);
	bounds[1] = glm::vec3(xmax, ymax, zmax);

	glm::vec3 invdir = glm::vec3(1 / ray.x, 1 / ray.y, 1 / ray.z);
	sign[0] = (invdir.x < 0);
	sign[1] = (invdir.y < 0);
	sign[2] = (invdir.z < 0);

	float tmin, tmax, txmin, txmax, tymin, tymax, tzmin, tzmax;

	txmin = (bounds[sign[0]].x - eye.x) * invdir.x;
	txmax = (bounds[1-sign[0]].x - eye.x) * invdir.x;
	tymin = (bounds[sign[1]].y - eye.y) * invdir.y;
	tymax = (bounds[1-sign[1]].y - eye.y) * invdir.y;
	tzmin = (bounds[sign[2]].z - eye.z) * invdir.z;
	tzmax = (bounds[1-sign[2]].z - eye.z) * invdir.z;

	// std::cout << txmin << " " << txmax << " " << tymin << " " << tymax << " " << tzmin << " " << tzmax << std::endl;

	tmin = glm::max(glm::max(txmin, tymin), tzmin);
	tmax = glm::min(glm::min(txmax, tymax), tzmax);

	// if (tmin > tmax || tmin < -kEpsilon) return new Intersection();
	if (tmin > tmax || (tmin < kEpsilon && tmax < kEpsilon)) return new Intersection();
	else if (tmin < kEpsilon) tmin = tmax;

	glm::vec3 normal;
	if (tmin > txmin - kEpsilon && tmin < txmin + kEpsilon) normal = glm::vec3(1, 0, 0) * (2 * sign[0] - 1);
	if (tmin > txmax - kEpsilon && tmin < txmax + kEpsilon) normal = glm::vec3(-1, 0, 0) * (2 * sign[0] - 1);
	if (tmin > tymin - kEpsilon && tmin < tymin + kEpsilon) normal = glm::vec3(0, 1, 0) * (2 * sign[1] - 1);
	if (tmin > tymax - kEpsilon && tmin < tymax + kEpsilon) normal = glm::vec3(0, -1, 0) * (2 * sign[1] - 1);
	if (tmin > tzmin - kEpsilon && tmin < tzmin + kEpsilon) normal = glm::vec3(0, 0, 1) * (2 * sign[2] - 1);
	if (tmin > tzmax - kEpsilon && tmin < tzmax + kEpsilon) normal = glm::vec3(0, 0, -1) * (2 * sign[2] - 1);

	glm::vec3 point = eye + tmin * ray;

	return new Intersection(true, tmin, point, normal);
}

NonhierBox::~NonhierBox()
{
}

Intersection* NonhierBox::intersect(const glm::vec3 &eye, const glm::vec3 &ray) {
	// glm::vec3 bounds[2];
	// int sign[3];

	// bounds[0] = m_pos;
	// bounds[1] = glm::vec3(m_pos.x + m_size, m_pos.y + m_size, m_pos.z + m_size);

	// glm::vec3 invdir = glm::vec3(1 / ray.x, 1 / ray.y, 1 / ray.z);
	// sign[0] = (invdir.x < 0);
	// sign[1] = (invdir.y < 0);
	// sign[2] = (invdir.z < 0);

	// float tmin, tmax, txmin, txmax, tymin, tymax, tzmin, tzmax;

	// txmin = (bounds[sign[0]].x - eye.x) * invdir.x;
	// txmax = (bounds[1-sign[0]].x - eye.x) * invdir.x;
	// tymin = (bounds[sign[1]].y - eye.y) * invdir.y;
	// tymax = (bounds[1-sign[1]].y - eye.y) * invdir.y;
	// tzmin = (bounds[sign[2]].z - eye.z) * invdir.z;
	// tzmax = (bounds[1-sign[2]].z - eye.z) * invdir.z;

	// tmin = glm::max(glm::max(txmin, tymin), tzmin);
	// tmax = glm::min(glm::min(txmax, tymax), tzmax);

	// glm::vec3 normal;
	// if (tmin > txmin - kEpsilon && tmin < txmin + kEpsilon) normal = glm::vec3(1, 0, 0);
	// if (tmin > txmax - kEpsilon && tmin < txmax + kEpsilon) normal = glm::vec3(-1, 0, 0);
	// if (tmin > tymin - kEpsilon && tmin < tymin + kEpsilon) normal = glm::vec3(0, 1, 0);
	// if (tmin > tymax - kEpsilon && tmin < tymax + kEpsilon) normal = glm::vec3(0, -1, 0);
	// if (tmin > tzmin - kEpsilon && tmin < tzmin + kEpsilon) normal = glm::vec3(0, 0, 1);
	// if (tmin > tzmax - kEpsilon && tmin < tzmax + kEpsilon) normal = glm::vec3(0, 0, -1);

	// if (tmin > tmax || tmin < kEpsilon) return new Intersection();

	// glm::vec3 point = eye + tmin * ray;

	// return new Intersection(true, tmin, point, normal);
	return b_box->intersect(eye, ray);
}

Cylinder::~Cylinder()
{
}

double planeIntersect(glm::vec3 point, glm::vec3 normal, glm::vec3 eye, glm::vec3 ray) {
	double num = glm::dot(point - eye, normal);
	double den = glm::dot(ray, normal);

	if (num == 0 && den == 0) {
		return std::numeric_limits<double>::max();
	} else if (den == 0) {
		return std::numeric_limits<double>::max();
	} else {
		return num / den;
	}
}

// Intersection* Cylinder::intersect(const glm::vec3 &eye, const glm::vec3 &ray) {
// 	double roots[2];
// 	double A = ray.x * ray.x + ray.z * ray.z;
// 	double B = 2 * eye.x * ray.x + 2 * eye.z * ray.z;
// 	double C = eye.x * eye.x + eye.z * eye.z - 1;
// 	size_t num = quadraticRoots(A, B, C, roots);

// 	if (num == 0) {
// 		return new Intersection();
// 	} else if (num == 1) {
// 		double t = roots[0];
// 		double y = eye.y + t * ray.y;
// 		if (y >= -1 && y <= 1) {
// 			glm::vec3 point = eye + t * ray;
// 			glm::vec3 normal = glm::normalize(glm::vec3(point.x, 0, point.z));
// 			return new Intersection(true, t, point, normal);
// 		} else {
// 			return new Intersection();
// 		}
// 	} else if (num == 2) {
// 		double t0 = std::min(roots[0], roots[1]);
// 		double t1 = std::max(roots[0], roots[1]);


// 		double y0 = eye.y + t0 * ray.y;
// 		double y1 = eye.y + t1 * ray.y;
// 		// std::cout << "t0 " << t0 << " t1 " << t1 << std::endl;
// 		// std::cout << "y0 " << y0 << " y1 " << y1 << std::endl;

// 		if (y0 < -1) {
// 			if (y1 < -1) { // Miss
// 				return new Intersection();
// 			} else { // Hits the cap at -1
// 				float th = t0 + (t1 - t0) * (y0 + 1) / (y0 - y1);
// 				if (th <= 0) {
// 					return new Intersection();
// 				} else {
// 					glm::vec3 point = eye + th * ray;
// 					glm::vec3 normal = glm::vec3(0, -1, 0);
// 					// std::cout << "eye " << to_string(eye) << " ray " << to_string(ray) << std::endl;
// 					// std::cout << "point " << to_string(point) << " normal " << to_string(normal) << std::endl;
// 					return new Intersection(true, th, point, normal);
// 				}
// 			}
// 		} else if (y0 >= -1 && y0 <= 1) {
// 			// Hit the cylinder bit
// 			if (t0 <= 0) {
// 				return new Intersection();
// 			} else {
// 				glm::vec3 point = eye + t0 * ray;
// 				glm::vec3 normal = glm::normalize(glm::vec3(point.x, 0, point.z));
// 				// std::cout << "eye " << to_string(eye) << " ray " << to_string(ray) << std::endl;
// 				// std::cout << "point " << to_string(point) << " normal " << to_string(normal) << std::endl;
// 				return new Intersection(true, t0, point, normal);
// 			}
// 		} else if (y0 > 1) {
// 			if (y1 > 1) {
// 				return new Intersection();
// 			} else {
// 				// Hit the cap
// 				float th = t0 + (t1 - t0) * (y0 - 1) / (y0 - y1);
// 				if (th <= 0) {
// 					return new Intersection();
// 				} else {
// 					glm::vec3 point = eye + th * ray;
// 					glm::vec3 normal = glm::vec3(0, 1, 0);
// 					return new Intersection(true, th, point, normal);
// 				}
// 			}
// 		}
// 	}

// 	return new Intersection();
// }

Intersection* Cylinder::intersect(const glm::vec3 &eye, const glm::vec3 &ray) {
	double roots[2];
	double A = ray.x * ray.x + ray.y * ray.y;
	double B = 2 * eye.x * ray.x + 2 * eye.y * ray.y;
	double C = eye.x * eye.x + eye.y * eye.y - 1;
	size_t num = quadraticRoots(A, B, C, roots);

	double zmax = 1, zmin = -1;
	double t = std::numeric_limits<double>::infinity();
	glm::vec3 normal;
	bool intersect = false;

	if (num > 0) {
		double t1 = roots[0];
		double t2 = (num == 1) ? std::numeric_limits<double>::infinity() : roots[1];

		if (t1 > kEpsilon || t2 > kEpsilon) {
			double z1 = eye.z + t1 * ray.z;
			double z2 = eye.z + t2 * ray.z;

			t = (z1 < zmax && z1 > zmin && t1 > 0) ? t1 : t;
			t = (z2 < zmax && z2 > zmin && t2 > 0) ? (t2 < t ? t2 : t) : t;

			if (t < std::numeric_limits<double>::max()) {
			// if (!std::isinf<double>(t)) {
				normal = eye + t * ray;
				normal.z = 0.0;
				intersect = true;
			}
		} 
	}

	Disc dmin(glm::vec3(0, 0, zmin), 1);
	Intersection *iend = dmin.intersect(eye, ray);
	if (iend->hit) {
		double tzmin = iend->t;
		if (tzmin < t) {
			t = tzmin;
			normal = glm::vec3(0, 0, -1);
			intersect = true;
		}
	}
	delete iend;

	Disc dmax(glm::vec3(0, 0, zmax), 1);
	iend = dmax.intersect(eye, ray);
	if (iend->hit) {
		double tzmax = iend->t;
		if (tzmax < t) {
			t = tzmax;
			normal = glm::vec3(0, 0, 1);
			intersect = true;
		}
	}
	delete iend;

	glm::vec3 point = eye + t * ray;
	if (intersect) return new Intersection(true, t, point, normal);
	else return new Intersection();
}

Disc::~Disc()
{
}

Intersection *Disc::intersect(const glm::vec3 &eye, const glm::vec3 &ray) {
	glm::vec3 normal = glm::vec3(0, 0, 1);

	// interscet with xy plane that containing the disk
	double den = glm::dot(normal, ray);
	if (fabs(den) < std::numeric_limits<double>::epsilon()) return new Intersection();

	// point cannot behind the ray
	double t = glm::dot(normal, m_pos - eye) * 1 / den;
	if (t < kEpsilon) return new Intersection();

	glm::vec3 point = eye + t * ray;

	// point need to be inside the disk
	if (glm::length(point - m_pos) > m_radius) return new Intersection();

	// flip normal if backface is facing the ray
	normal = glm::dot(eye - m_pos, normal) < 0 ? -normal : normal;

	return new Intersection(true, t, point, normal);
}
