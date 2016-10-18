#include "Command.hpp"

using namespace std;

Command::Command(std::vector<JointNode *> nodes, float angle)
  : nodes(nodes),
  	angle(angle),
  	prev_angles(nodes.size(), 0)
{
	int i = 0;
	for (auto node : nodes) {
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
	int i = 0;
	for (auto node : nodes) {
		if (node->m_joint_x.min == node->m_joint_x.max) { // y-axis
			node->rotate('y', angle);
		} else { // x-axis
			node->rotate('x', angle);
		}
		++i;
	}
}

void Command::undo() {
	int i = 0;
	for (auto node : nodes) {
		if (node->m_joint_x.min == node->m_joint_x.max) { // y-axis
			node->rotate('y', prev_angles[i] - node->angle_y);
		} else { // x-axis
			node->rotate('x', prev_angles[i] - node->angle_x);
		}
		++i;
	}
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
