#pragma once

#include <set>

#include "JointNode.hpp"

class Command {
public:
	Command(std::set<JointNode *> nodes, float angle);
	~Command() {};
	void execute();
	void undo();
	float angle;
	float prev_angle;
	std::set<JointNode *> nodes;
};
