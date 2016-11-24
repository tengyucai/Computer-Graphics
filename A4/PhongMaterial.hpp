#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess,
  	double transparency, double refraction_index);
  virtual ~PhongMaterial();

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
  double m_transparency;
  double m_refraction_index;

public:
	bool isDiffuse();
	bool isSpecular();
	bool isTransparent();

	glm::vec3 getkd();
	glm::vec3 getks();
	double getShininess();
	double getTransparency();
	double getRefractionIndex();
};
