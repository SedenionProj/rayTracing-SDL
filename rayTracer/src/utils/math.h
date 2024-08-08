#pragma once
#include <glm/glm.hpp>
#include "../camera.h"

inline float lerp(float t, float min, float max) {
	return (1.f - t) * min + t * max;
}

inline glm::vec3 orientNormal(glm::vec3& normal, glm::vec3& direction) {
	return  glm::dot(normal, direction) < 0.f ? normal: -normal;
}

inline glm::vec3 sphericalDirection(float sinTheta, float cosTheta, float phi) {
	return glm::vec3(sinTheta * std::cos(phi),
					 sinTheta* std::sin(phi),
				     cosTheta);
}

inline glm::mat3 xyz_to_rgb = glm::mat3(
	glm::vec3(3.2406, -0.9689, 0.0557),
	glm::vec3(-1.5372, 1.8758, -0.2040),
	glm::vec3(-0.4986, 0.0415, 1.0570));

class AABB {
public:
	AABB() : bMin{}, bMax{} {}

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

class CoordinateSystem {
public:
	CoordinateSystem(const glm::vec3& n) {
		w = glm::normalize(n);
		glm::vec3 a = (glm::abs(w.x) > 0.9) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
		v = glm::normalize(glm::cross(w, a));
		u = glm::cross(w, v);
	}

	glm::vec3 transform(const glm::vec3& vec) {
		return vec.x * u + vec.y * v + vec.z * w;
	}

	glm::vec3 invTransform(const glm::vec3& vec) {
		return glm::vec3(glm::dot(vec, u), glm::dot(vec, v), glm::dot(vec, w));
	}
private:
	glm::vec3 u,v, w;
};

