#pragma once
#include <glm/glm.hpp>
#include "../camera.h"

inline float lerp(float t, float min, float max) {
	return (1.f - t) * min + t * max;
}

inline glm::vec3 orientNormal(glm::vec3& normal, glm::vec3& direction) {
	return  glm::dot(normal, direction) < 0.f ? normal: -normal;
}

class AABB {
public:
	AABB(){}

	AABB(const AABB& a, const AABB& b)
		: bMin(glm::vec3(std::min(a.bMin.x, b.bMin.x), std::min(a.bMin.y, b.bMin.y), std::min(a.bMin.z, b.bMin.z))),
		  bMax(glm::vec3(std::max(a.bMax.x, b.bMax.x), std::max(a.bMax.y, b.bMax.y), std::max(a.bMax.z, b.bMax.z))) {};

	AABB(const glm::vec3& a, const glm::vec3& b)
		: bMin(glm::vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z))),
		  bMax(glm::vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z))) {};

	bool intersect(const Ray& ray, float tmin, float tmax) const {
		for (int a = 0; a < 3; ++a) {
			float invD = 1.0f / ray.direction[a];
			float t0 = (bMin[a] - ray.origin[a]) * invD;
			float t1 = (bMax[a] - ray.origin[a]) * invD;
			if (invD < 0.0f) {
				std::swap(t0, t1);
			}

			tmin = std::max(t0, tmin);
			tmax = std::min(t1, tmax);

			if (tmax <= tmin)
				return false;
		}

		return true;
	}

	glm::vec3 bMin;
	glm::vec3 bMax;
};

