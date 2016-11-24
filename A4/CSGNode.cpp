#include "CSGNode.hpp"

#include <glm/glm.hpp>
#include <glm/ext.hpp>

CSGNode::CSGNode( const std::string & name, GeometryNode *a, GeometryNode *b )
	: GeometryNode(name, NULL)
	, m_a(a)
	, m_b(b)
{
}
	
CSGNode::~CSGNode()
{
}

UnionNode::UnionNode( const std::string & name, GeometryNode *a, GeometryNode *b )
	: CSGNode(name, a, b)
{
}
	
UnionNode::~UnionNode()
{
}

Intersection *UnionNode::intersect(const glm::vec4 &eye, const glm::vec4 &ray)
{
	Intersection *intersection_a = m_a->intersect(invtrans * eye, invtrans * ray);
	Intersection *intersection_b = m_b->intersect(invtrans * eye, invtrans * ray);
	Intersection *i;

	if (intersection_a->hit && intersection_b->hit) {
		i = glm::distance(intersection_a->point, glm::vec3(eye)) < glm::distance(intersection_b->point, glm::vec3(eye)) ? intersection_a : intersection_b;
		i->material = (i == intersection_a) ? m_a->m_material : m_b->m_material;
	} else if (intersection_a->hit) {
		i = intersection_a;
		i->material = m_a->m_material;
	} else if (intersection_b->hit) {
		i = intersection_b;
		i->material = m_b->m_material;
	} else {
		return intersection_a;
	}

	if (m_material) i->material = m_material;
	i->point = glm::vec3(trans * glm::vec4(i->point, 1));
	i->normal = glm::vec3(glm::transpose(invtrans) * glm::vec4(i->normal, 0));

	return i;
}

IntersectionNode::IntersectionNode( const std::string & name, GeometryNode *a, GeometryNode *b )
	: CSGNode(name, a, b)
{
}
	
IntersectionNode::~IntersectionNode()
{
}

Intersection *IntersectionNode::intersect(const glm::vec4 &eye, const glm::vec4 &ray)
{
	Intersection *intersection_a = m_a->intersect(invtrans * eye, invtrans * ray);
	Intersection *intersection_b = m_b->intersect(invtrans * eye, invtrans * ray);
	Intersection *i;

	if (intersection_a->hit && intersection_b->hit) {
		i = glm::distance(intersection_a->point, glm::vec3(eye)) > glm::distance(intersection_b->point, glm::vec3(eye)) ? intersection_a : intersection_b;
		i->material = (i == intersection_a) ? m_a->m_material : m_b->m_material;
	} else {
		return new Intersection();
	}

	if (m_material) i->material = m_material;
	i->point = glm::vec3(trans * glm::vec4(i->point, 1));
	i->normal = glm::vec3(glm::transpose(invtrans) * glm::vec4(i->normal, 0));

	return i;
}

DifferenceNode::DifferenceNode( const std::string & name, GeometryNode *a, GeometryNode *b )
	: CSGNode(name, a, b)
{
}
	
DifferenceNode::~DifferenceNode()
{
}

Intersection *DifferenceNode::intersect(const glm::vec4 &eye1, const glm::vec4 &ray1)
{
	glm::vec4 eye = invtrans * eye1;
	glm::vec4 ray = invtrans * ray1;
	Intersection *intersection_a = m_a->intersect( eye,  ray);
	Intersection *intersection_b = m_b->intersect(eye,  ray);
	Intersection *i = NULL;

	if (intersection_a->hit) {
		double epsilon = std::numeric_limits<double>::epsilon();
		if (intersection_b->hit && glm::distance(intersection_b->point, glm::vec3(eye)) < glm::distance(intersection_a->point, glm::vec3(eye))) {
			int index = fabs(ray.x) > epsilon ? 0 : (fabs(ray.y) > epsilon ? 1 : 2);
			double t;
			if (index == 0) t = (intersection_a->point.x - eye.x) / ray.x;
			else if (index == 1) t = (intersection_a->point.y - eye.y) / ray.y;
			else t = (intersection_a->point.z - eye.z) / ray.z;

			// glm::vec4 new_eye = glm::vec4(intersection_a->point, 0) + 0.1 * ray;
			// std::cout << t << std::endl;
			// std::cout << intersection_a->t << std::endl;
			// std::cout << "eye " << glm::to_string(eye) << std::endl;
			// std::cout << "ray " << glm::to_string(ray) << std::endl;
			// std::cout << "point " << glm::to_string(intersection_a->point) << std::endl;
			// std::cout << "new eye " << glm::to_string(new_eye) << std::endl;
			glm::vec4 new_eye = eye + (t + 0.001) * ray;
			Intersection *intersection_u = m_a->intersect(new_eye, ray);
			Intersection *intersection_v = m_b->intersect(new_eye, ray);

			if (!intersection_v->hit) {
				i = intersection_a;
			} else if (intersection_u->hit) {
				// std::cout << "1st t " << intersection_u->t << std::endl;
				// std::cout << "2nd t " << intersection_v->t << std::endl;
				if ( glm::distance(intersection_v->point, glm::vec3(eye)) < glm::distance(intersection_u->point, glm::vec3(eye))) {
					i = intersection_v;
					// std::cout << "intersection_u->point " << glm::to_string(intersection_u->point) << std::endl;
					i->normal = -i->normal;
					// std::cout << "intersection_u->point " << glm::to_string(intersection_v->point) << std::endl;
				}
			}
		} else {
			i = intersection_a;
		}
	}
	
	if (i == NULL) return new Intersection();

	if (m_material) i->material = m_material;
	i->point = glm::vec3(trans * glm::vec4(i->point, 1));
	i->normal = glm::vec3(glm::transpose(invtrans) * glm::vec4(i->normal, 0));

	return i;
}
