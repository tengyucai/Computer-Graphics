#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"
#include "PerlinNoise.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess,
  	double transparency, double refraction_index, double bumpness);
  virtual ~PhongMaterial();

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
  double m_transparency;
  double m_refraction_index;
  double m_bumpness;

  PerlinNoise *m_perlin;

public:
	bool isDiffuse();
	bool isSpecular();
	bool isTransparent();

	glm::vec3 getkd();
	glm::vec3 getks();
	double getShininess();
	double getTransparency();
	double getRefractionIndex();
	double getBumpness();
	PerlinNoise *getpn();
	glm::vec3 bump(glm::vec3 n, glm::vec3 point);
};
