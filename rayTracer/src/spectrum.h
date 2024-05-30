#pragma once
#include "utils/sample.h"
#include "utils/math.h"

const float Lambda_min = 360, Lambda_max = 830;

const int nRGB2SpectSamples = 32;

class Spectrum {
public:
	virtual float operator()(float lambda) = 0;
};

class DenseSpectrum : public Spectrum {
public:
	DenseSpectrum(const float* values, int sampleNum): values(values, values+ sampleNum), sampleNum(sampleNum) {}

	float operator()(float lambda) override {
		int index = glm::round(sampleNum * (lambda - Lambda_min) / (Lambda_max - Lambda_min));
		return values[index];
	}
public:
	std::vector<float> values;
	float sampleNum;
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



extern const float RGBRefl2SpectCyan[nRGB2SpectSamples];

extern const float RGBRefl2SpectMagenta[nRGB2SpectSamples];

extern const float RGBRefl2SpectYellow[nRGB2SpectSamples];

extern DenseSpectrum Cyan;
extern DenseSpectrum Magenta;
extern DenseSpectrum Yellow;

extern DenseSpectrum X;
extern DenseSpectrum Y;
extern DenseSpectrum Z;