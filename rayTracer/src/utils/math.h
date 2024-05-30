#pragma once
#include <glm/glm.hpp>

inline float lerp(float t, float min, float max) {
	return (1.f - t) * min + t * max;
}