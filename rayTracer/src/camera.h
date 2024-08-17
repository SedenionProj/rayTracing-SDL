#pragma once
#include <glm/glm.hpp>
#include "utils/sample.h"
#include "utils/math.h"

class Ray {
public:
	glm::vec3 origin;
	glm::vec3 direction;
	
	glm::vec3 at(float t) {
		return origin + t * direction;
	}
};

struct Film {
	Film(const glm::uvec2& resolution):
		RGBBuffer(resolution.x* resolution.y, glm::vec3(0)),m_resolution(resolution) {}

	glm::vec3 getSample(const glm::uvec2& p) {
		return RGBBuffer[p.y * m_resolution.x + p.x];
	}

	void addSample(const glm::uvec2& p, glm::vec3 col) {
		RGBBuffer[p.y * m_resolution.x + p.x] += col;
	}

	void setSample(const glm::uvec2& p, glm::vec3 col) {
		totalSamplesNB = 1;
		RGBBuffer[p.y * m_resolution.x + p.x] = col;
	}

	std::vector<glm::vec3> RGBBuffer;
	glm::uvec2 m_resolution;

	unsigned int totalSamplesNB = 0;
};

class Camera {
public:
	Camera(const glm::vec3& position, const glm::vec3& direction, const glm::vec2 resolution):
		film(resolution), position(position), direction(direction), spp(2,2) {}

	Ray getRay(glm::uvec2 pixelCoord, Sampler& sampler) const {
		float focalLength = 1.;

		glm::vec3 fixedUp = glm::vec3(0, 1, 0);
		glm::vec3 front =   glm::normalize(direction);
		glm::vec3 right =  glm::normalize(glm::cross(front, fixedUp));
		glm::vec3 up = cross(front, right);

		float h = focalLength * glm::tan(glm::radians(vfov) / 2.);
 
		glm::vec2 uv = 2.f * ((glm::vec2)pixelCoord + antiAliasingOffset(sampler)*2.f) / (glm::vec2)film.m_resolution - 1.f;
		uv.x *= film.m_resolution.x / film.m_resolution.y;

		Ray ray;
		ray.origin = position;		
		ray.direction = glm::normalize(
			-h * uv.x * right + 
			h * uv.y * up + 
			front * focalLength);
		return ray;
	}

	void update() { film.totalSamplesNB += spp.x * spp.y; }

public:
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec2 spp;
	Film film;
	float vfov = 90;

private:
	glm::vec2 antiAliasingOffset(Sampler& sampler) const {
		return sampler.get2D() - 0.5f;
	}

};