#pragma once

#include <iosfwd>

#include <glm/glm.hpp>

// Represents a simple point light.
struct Light {
  Light();
  
  glm::vec3 colour;
  glm::vec3 position;
  double falloff[3];
  double power;
  double num_photons;
};

std::ostream& operator<<(std::ostream& out, const Light& l);
