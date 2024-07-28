#pragma once

#include "stb_image.h"
#include <glm/glm.hpp>
#include <iostream>

class Image {
public:
	Image(const char* path) {
		data = stbi_load(path, &width, &height, &channels, 3);
	}

	glm::vec3 pixelColor(glm::uvec2 pos) {
		pos = glm::clamp(pos, glm::uvec2(0), glm::uvec2(width,height));

		unsigned char* pixel = data + pos.y * channels*width + pos.x*channels;
		
		//std::cout << static_cast<int>(*pixel)<<" "<< static_cast<int>(*(pixel+1))<<" "<< static_cast<int>(*(pixel+2)) << "\n";
		return glm::vec3(static_cast<int>(*pixel), static_cast<int>(*(pixel + 1)), static_cast<int>(*(pixel + 2)));
	}
public:
	int width, height, channels = 0;
	unsigned char* data;
};