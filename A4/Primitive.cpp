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
		float t = roots[0];
		if (num == 2 && roots[1] < roots[0]) t = roots[1];
		if (t < kEpsilon) return new Intersection();
		glm::vec3 point = eye + t * ray;
		glm::vec3 normal = point - m_pos;
		return new Intersection(true, t, point, normal);
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

	glm::vec3 normal;
	if (tmin > txmin - kEpsilon && tmin < txmin + kEpsilon) normal = glm::vec3(1, 0, 0) * (2 * sign[0] - 1);
	if (tmin > txmax - kEpsilon && tmin < txmax + kEpsilon) normal = glm::vec3(-1, 0, 0) * (2 * sign[0] - 1);
	if (tmin > tymin - kEpsilon && tmin < tymin + kEpsilon) normal = glm::vec3(0, 1, 0) * (2 * sign[1] - 1);
	if (tmin > tymax - kEpsilon && tmin < tymax + kEpsilon) normal = glm::vec3(0, -1, 0) * (2 * sign[1] - 1);
	if (tmin > tzmin - kEpsilon && tmin < tzmin + kEpsilon) normal = glm::vec3(0, 0, 1) * (2 * sign[2] - 1);
	if (tmin > tzmax - kEpsilon && tmin < tzmax + kEpsilon) normal = glm::vec3(0, 0, -1) * (2 * sign[2] - 1);

	if (tmin > tmax || tmin < -kEpsilon) return new Intersection();

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
