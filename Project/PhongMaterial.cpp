#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, const glm::vec3& kt, double shininess )
	: m_kd(kd)
	, m_ks(ks)
	, m_kt(kt)
	, m_shininess(shininess)
	, m_refraction_index(1.0f)
{}

PhongMaterial::~PhongMaterial()
{}

glm::vec3 PhongMaterial::getkd() {
	return m_kd;
}

glm::vec3 PhongMaterial::getks() {
	return m_ks;
}

glm::vec3 PhongMaterial::getkt() {
	return m_kt;
}

double PhongMaterial::getShininess() {
	return m_shininess;
}

bool PhongMaterial::isDiffuse() {
	return m_kd.x > 0.0f || m_kd.y > 0.0f || m_kd.z > 0.0f;
}

bool PhongMaterial::isSpecular() {
	return m_ks.x > 0.0f || m_ks.y > 0.0f || m_ks.z > 0.0f;
}

bool PhongMaterial::isTransparent() {
	return m_kt.x > 0.0f || m_kt.y > 0.0f || m_kt.z > 0.0f;
}

double PhongMaterial::getRefractionIndex() {
	return m_refraction_index;
}
