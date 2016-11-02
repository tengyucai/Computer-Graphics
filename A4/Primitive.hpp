#pragma once

#include "Material.hpp"

#include <glm/glm.hpp>

class Intersection {
public:
  bool hit;
  glm::vec3 point;
  glm::vec3 normal;
  Material *material;
  float t;
  Intersection()
    : hit(false), point(glm::vec3()), normal(glm::vec3()), material(NULL), t(0) {}
  Intersection(bool hit, float t, glm::vec3 point, glm::vec3 normal)
    : hit(hit), t(t), point(point), normal(normal), material(NULL) {}
};

class Primitive {
public:
  virtual ~Primitive();

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

class Sphere : public Primitive {
public:
  Sphere() : n_sphere(new NonhierSphere(glm::vec3(0, 0, 0), 1)) {};
  virtual ~Sphere();

private:
  NonhierSphere *n_sphere;

public:
  Intersection virtual *intersect(const glm::vec3 &eye, const glm::vec3 &ray);
};

class Cube : public Primitive {
public:
  Cube() : n_box(new NonhierBox(glm::vec3(0, 0, 0), 1)) {};
  virtual ~Cube();

private:
  NonhierBox *n_box;

public:
  Intersection virtual *intersect(const glm::vec3 &eye, const glm::vec3 &ray);
};
