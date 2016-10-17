#include "Command.hpp"

using namespace std;

Command::Command(std::set<JointNode *> nodes, float angle)
  : nodes(nodes),
  	angle(angle),
  	prev_angle(0)
{

}

void Command::execute() {
	for (auto node = nodes.begin(); node != nodes.end(); ++node) {
		if (node->m_joint_x.min == node->m_joint_x.max) { // y-axis
			node->rotate('y', angle);
		} else { // x-axis
			node->rotate('x', angle);
		}
	}
}

void Command::undo() {

}
