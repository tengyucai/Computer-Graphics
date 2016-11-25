#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, 
	double transparency, double refraction_index )
	: m_kd(kd)
	, m_ks(ks)
	, m_transparency(transparency)
	, m_shininess(shininess)
	, m_refraction_index(refraction_index)
{}

PhongMaterial::~PhongMaterial()
{}

glm::vec3 PhongMaterial::getkd() {
	return m_kd;
}

glm::vec3 PhongMaterial::getks() {
	return m_ks;
}

double PhongMaterial::getShininess() {
	return m_shininess;
}

bool PhongMaterial::isDiffuse() {
	return m_kd.x > 0.01f || m_kd.y > 0.01f || m_kd.z > 0.01f;
}

bool PhongMaterial::isSpecular() {
	return m_ks.x > 0.01f || m_ks.y > 0.01f || m_ks.z > 0.01f;
}

bool PhongMaterial::isTransparent() {
	return m_transparency > 0;
}

double PhongMaterial::getTransparency() {
	return m_transparency;
}

double PhongMaterial::getRefractionIndex() {
	return m_refraction_index;
}
