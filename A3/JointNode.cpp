#include "JointNode.hpp"

#include "cs488-framework/MathUtils.hpp"

#include <iostream>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

using namespace glm;

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name),
	  angle_x(0),
	  angle_y(0)
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
	angle_x = init;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
	angle_y = init;
}

void JointNode::rotate(char axis, float angle) {
	vec3 rot_axis;

	switch (axis) {
		case 'x':
			rot_axis = vec3(1,0,0);
			if (angle_x + angle <= m_joint_x.min) angle_x = m_joint_x.min;
			else if (angle_x + angle >= m_joint_x.max) angle_x = m_joint_x.max;
			else angle_x += angle;
			angle = angle_x;
			break;
		case 'y':
			rot_axis = vec3(0,1,0);
			if (angle_y + angle <= m_joint_y.min) angle_y = m_joint_y.min;
			else if (angle_y + angle >= m_joint_y.max) angle_y = m_joint_y.max;
			else angle_y += angle;
			angle = angle_y;
	        break;
		case 'z':
			rot_axis = vec3(0,0,1);
	        break;
		default:
			break;
	}

	// mat4 rot_matrix = glm::rotate(degreesToRadians(angle), rot_axis);
	trans = glm::rotate(degreesToRadians(angle), rot_axis);
}
