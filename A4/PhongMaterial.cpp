#include "PhongMaterial.hpp"
#include <math.h>

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, 
	double transparency, double refraction_index, double bumpness)
	: m_kd(kd)
	, m_ks(ks)
	, m_transparency(transparency)
	, m_shininess(shininess)
	, m_refraction_index(refraction_index)
	, m_bumpness(bumpness)
	, m_perlin(new PerlinNoise())
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

double PhongMaterial::getBumpness() {
	return m_bumpness;
}

PerlinNoise* PhongMaterial::getpn() {
	return m_perlin;
}

glm::vec3 PhongMaterial::bump(glm::vec3 n, glm::vec3 point) {
	if (m_bumpness > 0) {
		glm::vec3 nn, pp;
		pp = 0.1f * point;
		double noiseCoefx = m_perlin->noise(pp.x, pp.y, pp.z);
		double noiseCoefy = m_perlin->noise(pp.y, pp.z, pp.x);
		double noiseCoefz = m_perlin->noise(pp.z, pp.x, pp.y);
		nn.x = (1.0f - m_bumpness) * nn.x + m_bumpness * noiseCoefx;
		nn.y = (1.0f - m_bumpness) * nn.y + m_bumpness * noiseCoefy;
		nn.z = (1.0f - m_bumpness) * nn.z + m_bumpness * noiseCoefz;
		double tmp = glm::dot(nn, nn);
		if (fabs(tmp - 0.0) < 0.0001) return n;
		tmp = 1.0 / sqrt(tmp);
		return (float) tmp + nn;
	}

	return n;
}
