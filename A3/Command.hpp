#pragma once

#include <vector>

#include "JointNode.hpp"

// class Command {
// public:
// 	Command(std::vector<JointNode *> nodes);
// 	~Command();
// 	void execute(float angle);
// 	void undo();
// 	void redo();
// 	//float angle;
// 	std::vector<float> prev_angles;
// 	std::vector<JointNode *> nodes;
// };

class Command {
public:
	Command(std::vector<JointNode *> nodes, float angle);
	~Command();
	void execute();
	void undo();
	float angle;
	std::vector<float> prev_angles;
	float prev_head_angle;
	std::vector<JointNode *> nodes;
	bool enable_joints;
	bool enable_head;
};
