#include "GeometryNode.hpp"

#include <iostream>
#include <glm/ext.hpp>
#include "PhongMaterial.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

Intersection* GeometryNode::intersect(const glm::vec4 &eye, const glm::vec4 &ray) {
	Intersection *intersection = m_primitive->intersect(glm::vec3(invtrans * eye), glm::vec3(invtrans * ray));
	//return m_primitive->intersect(glm::vec3(eye), glm::vec3(ray));
	intersection->material = m_material;
	intersection->point = glm::vec3(trans * glm::vec4(intersection->point, 1));
	PhongMaterial *material = (PhongMaterial *)m_material;
	glm::vec3 normal = intersection->normal;
	if (intersection->hit) {
		// std::cout << "point " << glm::to_string(intersection->point) << std::endl;
		// std::cout << "normal " << glm::to_string(intersection->normal) << std::endl;
		material->normalMap(intersection->uv.x, intersection->uv.y, normal);
	}
	intersection->normal = glm::vec3(glm::transpose(invtrans) * glm::vec4(normal, 0));
	// if (intersection->hit) std::cout << glm::to_string(intersection->normal) << std::endl;
	return intersection;
}
