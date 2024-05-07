#pragma once
#include <glm/glm.hpp>


class Ray {
public:
	glm::vec3 origin;
	glm::vec3 direction;
	
	glm::vec3 at(float t) {
		return origin + t * direction;
	}
};

class Camera {
public:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec2 resolution;
	float vfov;

	Ray getRay(glm::vec2 pixelCoord) {
		float focalLength = 1.;

		glm::vec3 fixedUp = glm::vec3(0, 1, 0);
		glm::vec3 front = glm::normalize(direction);
		glm::vec3 right = -glm::normalize(glm::cross(front, fixedUp));
		glm::vec3 up = cross(front, right);

		float h = focalLength * glm::tan(glm::radians(vfov) / 2.);
 
		glm::vec2 uv = 2.f * pixelCoord / resolution - 1.f;
		uv.x *= resolution.x / resolution.y;

		Ray ray;
		ray.origin = position;		
		ray.direction = glm::normalize(h * uv.x * right + h * uv.y * up + front * focalLength);
		return ray;
	}
};