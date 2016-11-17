#include "JointNode.hpp"

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
}

Intersection* JointNode::intersect(const glm::vec4 &eye, const glm::vec4 &ray) {
	Intersection *intersection = new Intersection();
	Intersection *tmp;
	float min_t = std::numeric_limits<float>::infinity();

	for (SceneNode *child : children) {
		tmp = child->intersect(invtrans * eye, invtrans * ray);
		if (tmp->hit && tmp->t < min_t) {
			min_t = tmp->t;
			delete intersection;
			intersection = tmp;
		}else {
			delete tmp;
		}
	}

	intersection->point = glm::vec3(trans * glm::vec4(intersection->point, 1));
	intersection->normal = glm::vec3(glm::transpose(invtrans) * glm::vec4(intersection->normal, 0));
	// if (intersection->hit) std::cout << glm::to_string(intersection->normal) << std::endl;
	return intersection;
}
