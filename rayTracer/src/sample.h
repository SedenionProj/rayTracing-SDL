#pragma once
#include <glm/glm.hpp>
#include "constant.h"

#include <random>

inline double random_double() {
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline glm::vec3 SampleUniformHemisphere() {
	float z = random_double();
	float r = glm::sqrt(1 - z*z);
	float phi = 2.f * PI * random_double();

	return { r * std::cos(phi), r * std::sin(phi), z };
}

inline glm::vec3 SampleUniformSphere() {
	float z = 1 - 2 * random_double();
	float r = glm::sqrt(1 - z*z);
	float phi = 2 * PI * random_double();
	return { r * std::cos(phi), r * std::sin(phi), z };
}