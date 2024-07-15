#pragma once
#include "utils/constant.h"
#include "shape.h"
#include "spectrum.h"

struct HitInfo;

class Material {
public:
	Material() {};
	virtual float getBSDF(glm::vec3 wo, glm::vec3 wi, HitInfo& rec, WaveLength lambda) = 0;
};

class Diffuse : public Material {
public:
	Diffuse(const Spectrum& spectrum): m_spectrum(spectrum) {}
	
	float getBSDF(glm::vec3 wo, glm::vec3 wi, HitInfo& rec, WaveLength lambda) override;

	const Spectrum& m_spectrum;
};