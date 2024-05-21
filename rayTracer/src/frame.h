#pragma once
#include <glm/glm.hpp>

class Frame {
public:
	Frame(glm::vec3 x, glm::vec3 y, glm::vec3 z)
		:x(x), y(y), z(z) {}
private:
	glm::vec3 x, y, z;
};