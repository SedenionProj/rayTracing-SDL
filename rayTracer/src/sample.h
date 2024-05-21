#pragma once
#include <glm/glm.hpp>
#include "constant.h"

inline glm::vec3 SampleUniformHemisphere() {
	float z = (rand() % 10000)/ 10000.f;
	float r = glm::sqrt(1 - z*z);
	float phi = 2.f * PI * (rand() % 10000) / 10000.f;

	return { r * std::cos(phi), r * std::sin(phi), z };
}

inline glm::vec3 SampleUniformSphere() {
	float z = 1 - 2 * (rand() % 10000) / 10000.f;
	float r = glm::sqrt(1 - z*z);
	float phi = 2 * PI * (rand() % 10000) / 10000.f;;
	return { r * std::cos(phi), r * std::sin(phi), z };
}