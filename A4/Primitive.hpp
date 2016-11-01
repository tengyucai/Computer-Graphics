#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

class Intersection {
public:
  bool hit;
  glm::vec3 point;
  glm::vec3 normal;
  Material *material;
  Intersection()
    : hit(false), point(glm::vec3()), normal(glm::vec3()), material(NULL) {}
  Intersection(bool hit, glm::vec3 point, glm::vec3 normal)
    : hit(hit), point(point), normal(normal), material(NULL) {}
};

class Primitive {
public:
  virtual ~Primitive();

public:
  Intersection virtual *intersect(const glm::vec3 &eye, const glm::vec3 &ray);
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();

public:
  Intersection virtual *intersect(const glm::vec3 &eye, const glm::vec3 &ray);
};

class Cube : public Primitive {
public:
  virtual ~Cube();

public:
  Intersection virtual *intersect(const glm::vec3 &eye, const glm::vec3 &ray);
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();

private:
  glm::vec3 m_pos;
  double m_radius;

public:
  Intersection virtual *intersect(const glm::vec3 &eye, const glm::vec3 &ray);
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;

public:
  Intersection virtual *intersect(const glm::vec3 &eye, const glm::vec3 &ray);
};
