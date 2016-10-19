#include "Command.hpp"

using namespace std;

Command::Command(std::vector<JointNode *> nodes, float angle)
  : nodes(nodes),
  	angle(angle),
  	prev_angles(nodes.size(), 0)
{
	int i = 0;
	for (auto node : nodes) {
		if (node->m_name == "headJoint") {
			prev_head_angle = node->angle_y;
		}
		if (node->m_joint_x.min == node->m_joint_x.max) { // y-axis
			prev_angles[i] = node->angle_y;
		} else { // x-axis
			prev_angles[i] = node->angle_x;
		}
		++i;
	}
}

Command::~Command()
{

}

void Command::execute() {
	if (enable_joints) {
		for (auto node : nodes) {
			if (node->m_name == "headJoint") {
				node->rotate('x', angle);
			} else if (node->m_joint_x.min == node->m_joint_x.max) { // y-axis
				node->rotate('y', angle);
			} else { // x-axis
				node->rotate('x', angle);
			}
		}
	}
	if (enable_head) {
		for (auto node : nodes) {
			if (node->m_name == "headJoint") node->rotate('y', angle);
		}
	}
	// for (auto node : nodes) {
	// 	if (node->m_joint_x.min == node->m_joint_x.max) { // y-axis
	// 		node->rotate('y', angle);
	// 	} else { // x-axis
	// 		node->rotate('x', angle);
	// 	}
	// 	++i;
	// }
}

void Command::undo() {
	if (enable_joints) {
		int i = 0;
		for (auto node : nodes) {
			if (node->m_name == "headJoint") {
				node->rotate('x', prev_angles[i] - node->angle_x);
			} else if (node->m_joint_x.min == node->m_joint_x.max) { // y-axis
				node->rotate('y', prev_angles[i] - node->angle_y);
			} else { // x-axis
				node->rotate('x', prev_angles[i] - node->angle_x);
			}
			++i;
		}
	}
	if (enable_head) {
		for (auto node : nodes) {
			if (node->m_name == "headJoint") node->rotate('y', prev_head_angle - node->angle_y);
		}
	}

	// int i = 0;
	// for (auto node : nodes) {
	// 	if (node->m_joint_x.min == node->m_joint_x.max) { // y-axis
	// 		node->rotate('y', prev_angles[i] - node->angle_y);
	// 	} else { // x-axis
	// 		node->rotate('x', prev_angles[i] - node->angle_x);
	// 	}
	// 	++i;
	// }
}

// void Command::redo() {
// 	int i = 0;
// 	for (auto it = nodes.begin(); it != nodes.end(); ++it, ++i) {
// 		JointNode *node = *it;
// 		if (node->m_joint_x.min == node->m_joint_x.max) { // y-axis
// 			node->rotate('y', prev_angles[i]);
// 		} else { // x-axis
// 			node->rotate('x', prev_angles[i]);
// 		}
// 	}
// }
