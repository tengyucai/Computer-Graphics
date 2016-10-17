#include "Command.h"

using namespace std;

Command::Command(std::set<JointNode *> nodes, float angle)
  : nodes(nodes),
  	angle(angle),
  	prev_angle(0)
{

}

void Command::execute() {
	for (auto it = nodes.begin(); it != nodes.end(); ++it) {
		
	}
}

void Command::undo() {

}
