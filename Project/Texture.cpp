#include "Texture.hpp"
#include <lodepng/lodepng.h>
#include <glm/ext.hpp>
#include <iostream>

using namespace std;
using namespace glm;

Texture::Texture()
	:valid(false)
	,width(0)
	,height(0)
{}

Texture::Texture(const string filename){
	unsigned error = lodepng::decode(image, width, height, filename);
	if (!error)
		valid = true;
	else{
		std::cout << "Decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		std::cout << "Error loading texture " << filename << std::endl;
		return;
	}
	// cout << "width " << width << " height " << height << endl;
}

vec3 Texture::getColour(unsigned x, unsigned y){
	// cout << "image x " << x << " image y " << y << endl;
	if (x > width || y > height) throw string("Requested pixel outside of texture");
	unsigned base = (y * width + x) * 4;
	// cout << "base " << base << endl;
	// cout << glm::to_string(vec3(float(image[base]) / 255, float(image[base+1]) / 255, float(image[base+2]) / 255)) << endl;
	return vec3(float(image[base]) / 255, float(image[base+1]) / 255, float(image[base+2]) / 255);
}

vec3 Texture::getColour(float x, float y){
	unsigned ix = x * width, iy = y * height;
	return getColour(ix, iy);
}

vec3 Texture::getColour(double x, double y){
	// cout << "u " << x << " v " << y << endl;
	unsigned ix = x * width, iy = y * height;
	if (ix == width) --ix;
	if (iy == height) --iy;
	// cout << "ix " << ix << " iy " << iy << endl;
	return getColour(ix, iy);
}
