#include <iostream>
#include <fstream>
#include <string>
#include <limits>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

#define kEpsilon 0.000001

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
  , m_face_normals()
  , m_vts()
{
	std::string code;
	double vx, vy, vz;
  double vtu, vtv;
	size_t s1, s2, s3;
  size_t uv1, uv2, uv3;
  std::string str1, str2, str3;
  double xmin = std::numeric_limits<double>::max();
  double xmax = -std::numeric_limits<double>::max();
  double ymin = std::numeric_limits<double>::max();
  double ymax = -std::numeric_limits<double>::max();
  double zmin = std::numeric_limits<double>::max();
  double zmax = -std::numeric_limits<double>::max();
  is_plane = false;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;

      xmin = fmin(xmin, vx);
      xmax = fmax(xmax, vx);
      ymin = fmin(ymin, vy);
      ymax = fmax(ymax, vy);
      zmin = fmin(zmin, vz);
      zmax = fmax(zmax, vz);
      // std::cout << xmin << " " << xmax << " " << ymin << " " << ymax << " " << zmin << " " << zmax << std::endl;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		// } else if( code == "f" ) {
		// 	ifs >> s1 >> s2 >> s3;
		// 	m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		} else if ( code == "f" ) {
      ifs >> str1 >> str2 >> str3;
      size_t found1 = str1.find_first_of("/");
      size_t found2 = str2.find_first_of("/");
      size_t found3 = str3.find_first_of("/");
      if (found1 != std::string::npos) {
        s1 = (size_t)std::stoi(str1);
        uv1 = (size_t)std::stoi(str1.substr(found1+1));
        s2 = (size_t)std::stoi(str2);
        uv2 = (size_t)std::stoi(str2.substr(found2+1));
        s3 = (size_t)std::stoi(str3);
        uv3 = (size_t)std::stoi(str3.substr(found3+1));
        m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
        m_face_normals.push_back( Triangle(uv1 - 1, uv2 - 1, uv3 - 1) );
      } else {
        s1 = (size_t)std::stoi(str1);
        s2 = (size_t)std::stoi(str2);
        s3 = (size_t)std::stoi(str3);
        m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
      }
    } else if ( code == "vt" ) {
      ifs >> vtu >> vtv;
      m_vts.push_back( std::make_pair(vtu, vtv) );
    }
	}

  std::cout << m_faces.size() << std::endl;
  std::cout << m_face_normals.size() << std::endl;
  std::cout << m_vertices.size() << std::endl;
  std::cout << m_vts.size() << std::endl;

  is_plane = (fabs(xmax - xmin) < kEpsilon) || (fabs(ymax - ymin) < kEpsilon) || (fabs(zmax - zmin) < kEpsilon);
  double size = fmax(xmax - xmin, fmax(ymax - ymin, zmax - zmin));
  // b_box = new NonhierBox(glm::vec3(xmin, ymin, zmin), size);
  b_box = new BoundedBox(xmin, xmax, ymin, ymax, zmin, zmax);
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

Intersection* Mesh::rayTriangleIntersect(const glm::vec3 &eye, const glm::vec3 &ray, 
    const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2) {
  glm::vec3 e1, e2, h, s, q;
  float a, f, u, v, t;

  e1 = v1 - v0;
  e2 = v2 - v0;

  h = glm::cross(ray, e2);
  a = glm::dot(e1, h);

  if (fabs(a) < kEpsilon) return new Intersection();

  f = 1 / a;
  s = eye - v0;
  u = f * glm::dot(s, h);

  if (u < 0.0 || u > 1.0) return new Intersection();
  // std::cout << "2" << std::endl;

  q = glm::cross(s, e1);
  v = f * glm::dot(ray, q);

  if (v < 0.0 || u + v > 1.0) return new Intersection();

  t = f * glm::dot(e2, q);

  if (t < kEpsilon) return new Intersection();
  // std::cout << "4" << std::endl;
  else {
    glm::vec3 point = eye + t * ray;
    glm::vec3 normal = glm::cross(e1, e2);
    bool inside = glm::dot(normal, ray) > 0;
    // std::cout << "inside " << inside << std::endl;
    Intersection *intersection = new Intersection(true, t, point, normal);
    intersection->inside = inside;
    return intersection;
  }
}

// Intersection* Mesh::rayTriangleIntersect(const glm::vec3 &eye, const glm::vec3 &ray, 
//     const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2) {
//   glm::vec3 e1, e2, h, s, q, normal;
//   float a, f, u, v, t;

//   // computer plane's normal
//   glm::vec3 v0v1 = v1 - v0;
//   glm::vec3 v0v2 = v2 - v0;
//   // no need t0 normalize
//   glm::vec3 N = glm::cross(v0v1, v0v2);
//   float denom = glm::dot(N, N);

//   // Step 1: finding P

//   // check if ray and plane are parallel
//   float NdotRayDir = glm::dot(N, ray);
//   if (fabs(NdotRayDir) < kEpsilon) // almost 0
//     return new Intersection();  // they are parallel so not intersect

//   // compute d parameter
//   float d = glm::dot(N, v0);

//   // compute t
//   t = (dot(N, eye) + d) / NdotRayDir;
//   // check if the triangle is behind the ray
//   if (t < 0) return new Intersection();

//   // compute the intersection point
//   glm::vec3 P = eye + t * ray;

//   // Step 2: inside-outside test
//   glm::vec3 C;  // vector perpendicular to triangle's plane

//   // edge 0
//   glm::vec3 edge0 = v1 - v0;
//   glm::vec3 vp0 = P - v0;
//   C = glm::cross(edge0, vp0);
//   if (glm::dot(N, C) < 0) return new Intersection(); // P is on the right side

//   // edge 1
//   glm::vec3 edge1 = v2 - v1;
//   glm::vec3 vp1 = P - v1;
//   C = glm::cross(edge1, vp1);
//   if (glm::dot(N, C) < 0) return new Intersection(); // P is on the right side

//   // edge 2
//   glm::vec3 edge2 = v0 - v2;
//   glm::vec3 vp2 = P - v2;
//   C = glm::cross(edge2, vp2);
//   if (glm::dot(N, C) < 0) return new Intersection(); // P is on the right side

//   return new Intersection(true, t, P, N);
// }

Intersection* Mesh::intersect(const glm::vec3 &eye, const glm::vec3 &ray) {
  Intersection *intersection = new Intersection();
  if (!is_plane) {
    Intersection *bbox_intersection = b_box->intersect(eye, ray);
    if (!bbox_intersection->hit) return intersection;
    // else return intersection;
  }

  Intersection *tmp;
  float min_t = std::numeric_limits<float>::infinity();;

  for (Triangle face : m_faces) {
    tmp = rayTriangleIntersect(eye, ray, m_vertices[face.v1], m_vertices[face.v2], m_vertices[face.v3]);
    if (tmp->hit && tmp->t < min_t) {
      // std::cout << "hit" << std::endl;
      delete intersection;
      intersection = tmp;
      min_t = tmp->t;
    } else {
      // std::cout << "not hit" << std::endl;
      delete tmp;
    }
	}
	return intersection;
}
