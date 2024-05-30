#pragma once
#include "constant.h"
#include "shape.h"
#include "spectrum.h"

class HitInfo;

class Material {
public:
	Material(glm::vec3 color) :m_color(color) {};
	virtual float getBSDF(glm::vec3 wo, glm::vec3 wi, HitInfo& rec, WaveLength lambda) = 0;
	glm::vec3 m_color;
};

class Diffuse : public Material {
public:
	Diffuse(glm::vec3 color, Spectrum& spectrum): Material(color), m_spectrum(spectrum) {
	}
	
	float getBSDF(glm::vec3 wo, glm::vec3 wi, HitInfo& rec, WaveLength lambda) override;

	Spectrum& m_spectrum;
};