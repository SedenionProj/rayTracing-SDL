#pragma once
#include "light.h"
#include "spectrum.h"
#include "shape.h"

Sky::Sky(float T, float power) :
		m_spec(new BlackBodySpectrum(T)), m_power(power) {}

LightSample Sky::sampleLi(glm::vec2 sample, glm::vec3 pos, float lambda)
{
	glm::vec3 wi = SampleUniformSphere(sample);

	return {Le(lambda), wi, 1.f/(4.f*PI), wi*1000000.f};
}

float Sky::LiPDF(glm::vec3 pos, glm::vec3 wi)
{
	return 1.f / (4.f * PI);
}

float Sky::Le(float lambda) {
	return m_power * (*m_spec)(lambda);
}

AreaLight::AreaLight(float T, float power, std::shared_ptr<Shape> shape) :
	m_spec(new BlackBodySpectrum(T)), m_power(power), m_shape(shape) {}

LightSample AreaLight::sampleLi(glm::vec2 sample, glm::vec3 pos, float lambda) {
	auto [Lp, pdf] = m_shape->sample(sample, pos);

	glm::vec3 wi = glm::normalize(Lp - pos);
	float Lemit = Le(lambda);
	return LightSample{ Lemit, wi, pdf,Lp };
}

float AreaLight::LiPDF(glm::vec3 pos, glm::vec3 wi) {
	return 0;
}

float AreaLight::Le(float lambda) {
	return m_power * (*m_spec)(lambda);
}
