#pragma once
#include <iostream>
#include "utils/sample.h"
#include "utils/math.h"

const int Lambda_min = 360, Lambda_max = 830;

class DenseSpectrum;
class SampledSpectrum;

extern DenseSpectrum green;
extern DenseSpectrum red;
extern DenseSpectrum white;

extern DenseSpectrum X;
extern DenseSpectrum Y;
extern DenseSpectrum Z;

extern DenseSpectrum light;

extern SampledSpectrum RGBRefl2SpectWhite;
extern SampledSpectrum RGBRefl2SpectCyan;
extern SampledSpectrum RGBRefl2SpectMagenta;
extern SampledSpectrum RGBRefl2SpectYellow;
extern SampledSpectrum RGBRefl2SpectRed;
extern SampledSpectrum RGBRefl2SpectGreen;
extern SampledSpectrum RGBRefl2SpectBlue;

class Spectrum {
public:
	virtual float operator()(float lambda) const = 0;
};

class DenseSpectrum : public Spectrum {
public:
	DenseSpectrum(const float* values, int l_min, int l_max)
		: m_values(values, values + l_max - l_min +1 ),
		l_min(l_min), l_max(l_max){}

	float operator()(float lambda) const override {
		if (lambda < l_min || lambda>l_max)
			return 0;
		int index = glm::round(lambda - l_min);
		return m_values[index];
	}
private:
	std::vector<float> m_values;
	int l_min, l_max;
};

class SampledSpectrum : public Spectrum {
public:
	SampledSpectrum(int n)
		: m_lambda(n, 0), m_values(n, 1) {}

	SampledSpectrum(const float* lambda, const float* value, int n)
		: m_lambda(lambda, lambda + n), m_values(value, value + n) {}

	SampledSpectrum(const glm::vec3& rgb)
		: m_lambda(RGBRefl2SpectWhite.m_lambda),
		m_values(RGBRefl2SpectWhite.m_lambda.size(), 0){
		if (rgb[0] <= rgb[1] && rgb[0] <= rgb[2]) {
			*this += rgb[0] * RGBRefl2SpectWhite;
			if (rgb[1] <= rgb[2]) {
				*this += (rgb[1] - rgb[0]) * RGBRefl2SpectCyan;
				*this += (rgb[2] - rgb[1]) * RGBRefl2SpectBlue;
			}
			else {
				*this += (rgb[2] - rgb[0]) * RGBRefl2SpectCyan;
				*this += (rgb[1] - rgb[2]) * RGBRefl2SpectGreen;
			}

		}
		else if (rgb[1] <= rgb[0] && rgb[1] <= rgb[2]) {
			*this += rgb[1] * RGBRefl2SpectWhite;
			if (rgb[0] <= rgb[2]) {
				*this += (rgb[0] - rgb[1]) * RGBRefl2SpectMagenta;
				*this += (rgb[2] - rgb[0]) * RGBRefl2SpectBlue;
			}
			else {
				*this += (rgb[2] - rgb[1]) * RGBRefl2SpectMagenta;
				*this += (rgb[0] - rgb[2]) * RGBRefl2SpectRed;
			}

		}
		else {
			*this += rgb[2] * RGBRefl2SpectWhite;
			if (rgb[0] <= rgb[1]) {
				*this += (rgb[0] - rgb[2]) * RGBRefl2SpectYellow;
				*this += (rgb[1] - rgb[0]) * RGBRefl2SpectGreen;
			}
			else {
				*this += (rgb[1] - rgb[2]) * RGBRefl2SpectYellow;
				*this += (rgb[0] - rgb[1]) * RGBRefl2SpectRed;
			}

		}
	}

	float operator()(float lambda) const override {
		auto val = std::upper_bound(m_lambda.begin(), m_lambda.end(), lambda);
		size_t index = val - m_lambda.begin();
		if (index < 0 || index>31)
			return 0;
		return m_values[index];
	}

	float operator[](int i) const {
		return m_values[i];
	}

	SampledSpectrum& operator*=(float c) {
		for (auto& v : this->m_values) {
			v *= c;
		}
		return *this;
	}

	SampledSpectrum& operator+=(const SampledSpectrum& spec) {
		for (int i = 0; i < m_lambda.size(); i++) {
			m_values[i] += spec[i];
		}
		return *this;
	}

	SampledSpectrum operator*(float c) const {
		SampledSpectrum spec = *this;
		return spec *= c;
	}

	friend SampledSpectrum operator*(float c,const SampledSpectrum& spectrum) {
		SampledSpectrum spec = spectrum;
		return spec *=c;
	}
	
	SampledSpectrum operator+(const SampledSpectrum& spectrum) {
		SampledSpectrum spec = *this;
		return *this += spectrum;
	}

public:
	std::vector<float> m_lambda;
	std::vector<float> m_values;
};

class RGBSpectrum : public Spectrum {
public:
	RGBSpectrum(const glm::vec3& rgb) {

	}

	float operator()(float lambda) const override {
		return 0;
	}

private:

};

class BlackBodySpectrum : public Spectrum {
public:
	BlackBodySpectrum(float T):
		m_T(T) {
		float lambdaMax = 2.8977721e-3f / T;
		m_normalizeFactor = 1 / blackbody(lambdaMax * 1e9f, T);
	}

	float operator()(float lambda) const override {
		return blackbody(lambda, m_T) * m_normalizeFactor;
	}

private:
	float blackbody(float lambda, float T) const {
		if (T <= 0) return 0;
		const float c = 299792458.f;
		const float h = 6.62606957e-34f;
		const float kb = 1.3806488e-23f;
		float l = lambda * 1e-9f;
		float Le = (2 * h * c * c) /
			(glm::pow(l,5) * (glm::exp((h * c) / (l * kb * T)) - 1));
		return Le;
	}

	float m_T;
	float m_normalizeFactor;
};

class WaveLength {
public:
	WaveLength() {
		float rdm = randomUniform();
		lambda = lerp(rdm, Lambda_min, Lambda_max);
	}
public:
	float lambda;
};



