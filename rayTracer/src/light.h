#pragma once
#include "spectrum.h"

class Shape;

struct LightSample {
	float L;
	glm::vec3 wi;
	float pdf;
	glm::vec3 pos;
};

class Light {
public:
	virtual float Le(float lambda) = 0;
	virtual LightSample sampleLi(glm::vec2 sample, glm::vec3 pos, float lambda) = 0;
	virtual float LiPDF(glm::vec3 pos, glm::vec3 wi) = 0;
};

class Sky : public Light {
public:
	Sky(float T, float power);

	LightSample sampleLi(glm::vec2 sample, glm::vec3 pos, float lambda) override;
	float LiPDF(glm::vec3 pos, glm::vec3 wi) override;
	float Le(float lambda) override;
private:
	float m_power;
	Spectrum* m_spec;
};

class AreaLight : public Light{
public:
	AreaLight(float T, float power, std::shared_ptr<Shape> shape);

	LightSample sampleLi(glm::vec2 sample, glm::vec3 pos, float lambda) override;

	float LiPDF(glm::vec3 pos, glm::vec3 wi) override;

	float Le(float lambda) override;
private:
	std::shared_ptr<Shape> m_shape;
	float m_power;
	Spectrum* m_spec;
};