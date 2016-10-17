#pragma once

#include <set>

#include "JointNode.h"

class Command {
public:
	Command(std::set<JointNode *> nodes, float angle);
	~Command() {};
	void execute();
	void undo();
private:
	float angle;
	float prev_angle;
	std::set<JointNode *> nodes;
};
