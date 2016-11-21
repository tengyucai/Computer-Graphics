#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, const glm::vec3& kt, double shininess);
  virtual ~PhongMaterial();

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;
  glm::vec3 m_kt;

  double m_shininess;
  double m_refraction_index;

public:
	bool isDiffuse();
	bool isSpecular();
	bool isTransparent();

	glm::vec3 getkd();
	glm::vec3 getks();
	glm::vec3 getkt();
	double getShininess();
	double getRefractionIndex();
};
