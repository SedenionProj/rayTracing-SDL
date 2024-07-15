#pragma once
#include <glm/glm.hpp>
#include "constant.h"

#include <random>

inline double randomUniform() {
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

inline int randInt(const int min, const int max) {
	return min + rand() % (max - min + 1);
}

inline glm::vec3 SampleUniformHemisphere() {
	float z = randomUniform();
	float r = glm::sqrt(1 - z*z);
	float phi = 2.f * PI * randomUniform();

	return { r * std::cos(phi), r * std::sin(phi), z };
}

inline glm::vec3 SampleUniformSphere() {
	float z = 1 - 2 * randomUniform();
	float r = glm::sqrt(1 - z*z);
	float phi = 2 * PI * randomUniform();
	
	return { r * std::cos(phi), r * std::sin(phi), z };
}

class RandomNumber {
public:
	RandomNumber(const uint64_t seed) {
		mcg_state = 2 * seed + 1;
		(void)pcg32_fast();
	}

	[[nodiscard]] float uniform() {
		// return range [0,1)
		return std::min(ONE_MINUS_EPSILON, (float)pcg32_fast() / MAX_UINT32);
	}

private:

	uint32_t pcg32_fast(void)
	{
		uint64_t x = mcg_state;
		unsigned count = (unsigned)(x >> 61);	// 61 = 64 - 3

		mcg_state = x * multiplier;
		x ^= x >> 22;
		return (uint32_t)(x >> (22 + count));	// 22 = 32 - 3 - 7
	}

	uint64_t mcg_state = 0xcafef00dd15ea5e5u;	// Must be odd
	uint64_t const multiplier = 6364136223846793005u;
};

class Sampler {
public:
	Sampler() : rng(0) {}

	[[nodiscard]] virtual float get1D() = 0;
	[[nodiscard]] virtual glm::vec2 get2D() = 0;
protected:
	RandomNumber rng;
};

class IndependantSampler : public Sampler {
public:
	IndependantSampler() {}

	float get1D() override {
		return rng.uniform();
	}

	glm::vec2 get2D() override {
		return glm::vec2(get1D(), get1D());
	}
};