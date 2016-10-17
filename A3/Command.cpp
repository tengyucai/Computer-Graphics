#include "Command.hpp"

using namespace std;

Command::Command(std::set<JointNode *> nodes, float angle)
  : nodes(nodes),
  	angle(angle)
{

}

Command::~Command()
{

}

void Command::execute() {
	for (auto it = nodes.begin(); it != nodes.end(); ++it) {
		JointNode *node = *it;
		if (node->m_joint_x.min == node->m_joint_x.max) { // y-axis
			// if (angle <= node->m_joint_y.min) {
			// 	/* code */
			// }
			node->rotate('y', angle);
		} else { // x-axis
			node->rotate('x', angle);
		}
	}
}

void Command::undo() {
	for (auto it = nodes.begin(); it != nodes.end(); ++it) {
		JointNode *node = *it;
		if (node->m_joint_x.min == node->m_joint_x.max) { // y-axis
			// if (angle <= node->m_joint_y.min) {
			// 	/* code */
			// }
			node->rotate('y', -angle);
		} else { // x-axis
			node->rotate('x', -angle);
		}
	}
}
