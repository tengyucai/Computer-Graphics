#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

class Texture{
public:
	Texture();
	Texture(const std::string filename);
	bool isValid(){ return valid; }
	glm::vec3 getColour(unsigned x, unsigned y);
	glm::vec3 getColour(float x, float y);
	glm::vec3 getColour(double x, double y);
private:
	std::vector<unsigned char> image;
	unsigned height, width;
	bool valid;
};
