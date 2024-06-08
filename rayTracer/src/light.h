#pragma once
#include "spectrum.h"

class Sky {
public:
	Sky(float T, float power) :
		m_spec(new BlackBodySpectrum(T)), m_power(power) {}

	float Le(float lambda) {
		return m_power * (*m_spec)(lambda);
	}
private:
	float m_power;
	Spectrum* m_spec;
};

class AreaLight {
public:
	AreaLight(float T, float power) :
		m_spec(new BlackBodySpectrum(T)), m_power(power) {}

	float Le(float lambda) {
		return m_power * (*m_spec)(lambda);
	}
private:
	float m_power;
	Spectrum* m_spec;
};