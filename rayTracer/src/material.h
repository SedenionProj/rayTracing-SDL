#pragma once
#include "constant.h"
#include "shape.h"

class HitInfo;

class Material {
public:
	virtual glm::vec3 getBSDF(glm::vec3 wo, glm::vec3 wi, HitInfo& rec) = 0;
	glm::vec3 m_color;
};

class Diffuse : public Material {
public:
	Diffuse(glm::vec3 color) {
		m_color = color;
	}

	glm::vec3 getBSDF(glm::vec3 wo, glm::vec3 wi, HitInfo& rec) override {
		if (glm::dot(wi, wi) < 0) {
			return glm::vec3(0);
		}
		return m_color / PI;
	};

};