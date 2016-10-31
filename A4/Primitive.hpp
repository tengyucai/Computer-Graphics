#pragma once

#include <glm/glm.hpp>

class Primitive {
public:
  virtual ~Primitive();

public:
  bool virtual intersect(const glm::vec3 &eye, const glm::vec3 &ray);
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();

public:
  bool virtual intersect(const glm::vec3 &eye, const glm::vec3 &ray);
};

class Cube : public Primitive {
public:
  virtual ~Cube();

public:
  bool virtual intersect(const glm::vec3 &eye, const glm::vec3 &ray);
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
  bool virtual intersect(const glm::vec3 &eye, const glm::vec3 &ray);
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
  bool virtual intersect(const glm::vec3 &eye, const glm::vec3 &ray);
};
