#pragma once
#include <glm/glm.hpp>
#include "constant.h"
#include <iostream>
#include <random>

#define DEFAULT_SEED 0x7fb5d329728ea185u

inline uint32_t MurmurHash3(const void* key, int len, uint32_t seed) {
	const uint32_t c1 = 0xcc9e2d51;
	const uint32_t c2 = 0x1b873593;

	uint32_t h1 = seed;
	const uint32_t* blocks = static_cast<const uint32_t*>(key);
	const int nblocks = len / 4;

	for (int i = 0; i < nblocks; i++) {
		uint32_t k1 = blocks[i];

		k1 *= c1;
		k1 = (k1 << 15) | (k1 >> (32 - 15));
		k1 *= c2;

		h1 ^= k1;
		h1 = (h1 << 13) | (h1 >> (32 - 13));
		h1 = h1 * 5 + 0xe6546b64; // Mix constant
	}

	const uint8_t* tail = static_cast<const uint8_t*>(key) + nblocks * 4;
	uint32_t k1 = 0;

	switch (len & 3) {
	case 3: k1 ^= tail[2] << 16;
	case 2: k1 ^= tail[1] << 8;
	case 1: k1 ^= tail[0];
		k1 *= c1;
		k1 = (k1 << 15) | (k1 >> (32 - 15));
		k1 *= c2;
		h1 ^= k1;
	}

	h1 ^= len;
	h1 ^= (h1 >> 16);
	h1 *= 0x85ebca6b;
	h1 ^= (h1 >> 13);
	h1 *= 0xc2b2ae35;
	h1 ^= (h1 >> 16);

	return h1;
}

template <typename... Args>
inline uint32_t hash(Args... args) {
	unsigned char arr[] = { static_cast<unsigned char>(args)... };
	return MurmurHash3(arr, sizeof...(args), 0);
}

inline int permutationElement(uint32_t i, uint32_t l, uint32_t p) {
	uint32_t w = l - 1;
	w |= w >> 1;
	w |= w >> 2;
	w |= w >> 4;
	w |= w >> 8;
	w |= w >> 16;
	do {
		i ^= p;
		i *= 0xe170893d;
		i ^= p >> 16;
		i ^= (i & w) >> 4;
		i ^= p >> 8;
		i *= 0x0929eb3f;
		i ^= p >> 23;
		i ^= (i & w) >> 1;
		i *= 1 | p >> 27;
		i *= 0x6935fa69;
		i ^= (i & w) >> 11;
		i *= 0x74dcb303;
		i ^= (i & w) >> 2;
		i *= 0x9e501cc3;
		i ^= (i & w) >> 2;
		i *= 0xc860a3df;
		i &= w;
		i ^= i >> 5;
	} while (i >= l);
	return (i + p) % l;
}

inline int randInt(const int min, const int max) {
	return min + rand() % (max - min + 1);
}

inline glm::vec3 SampleUniformHemisphere(glm::vec2 seed) {
	float z = seed.x;
	float r = glm::sqrt(1 - z*z);
	float phi = 2.f * PI * seed.y;

	return { r * std::cos(phi), r * std::sin(phi), z };
}

inline glm::vec3 SampleUniformSphere(glm::vec2 seed) {
	float z = 1 - 2 * seed.x;
	float r = glm::sqrt(1 - z*z);
	float phi = 2 * PI * seed.y;
	
	return { r * std::cos(phi), r * std::sin(phi), z };
}

#include <mutex>
static std::mutex cout_mutex;


class RNG {
public:
	
	RNG(const uint64_t seed = DEFAULT_SEED) {
		mcg_state = 2 * seed + 1;
	}

	[[nodiscard]] float uniform() {
		// return range [0,1)
		return std::min(ONE_MINUS_EPSILON, pcg32_fast() * 0x1p-32f);
	}

	void setSeed(const uint64_t seed) {
		mcg_state += 2 * seed + 1;
		(void)pcg32_fast();
	}

private:

	uint32_t pcg32_fast(void)
	{
		uint64_t x = mcg_state;
		unsigned count = (unsigned)(x >> 61);

		mcg_state = x * multiplier;
		x ^= x >> 22;

		return (uint32_t)(x >> (22 + count));
	}

	uint64_t mcg_state = 0xcafef00dd15ea5e5u;
	uint64_t const multiplier = 6364136223846793005u;
};

class Sampler {
public:
	Sampler() {}

	[[nodiscard]] virtual float get1D() = 0;
	[[nodiscard]] virtual glm::vec2 get2D() = 0;

	virtual void startSample(glm::uvec2 pixelPos, int sampleIndex, int seed = 0) = 0;
	virtual int samplesPerPixel() = 0;
protected:
	RNG rng;
	glm::uvec2 m_pixelPos;
	int m_seed;
};

class IndependantSampler : public Sampler {
public:
	IndependantSampler(int samplesPerPixel) 
		: m_samplesNB(samplesPerPixel){}

	float get1D() override {	
		return  rng.uniform();
	}

	glm::vec2 get2D() override {
		return glm::vec2(get1D(), get1D());
	}

	void startSample(glm::uvec2 pixelPos, int sampleIndex, int seed) override {
		rng.setSeed(rand());
	}

	int samplesPerPixel() override { return m_samplesNB; }

private:
	int m_samplesNB;
};

class StratifiedSampler : public Sampler {
public:
	StratifiedSampler(const int xSamplesNB, const int ySamplesNB)
		: m_xSamplesNB(xSamplesNB), m_ySamplesNB(ySamplesNB) {}

	float get1D() override {
		uint32_t hash_ = hash(m_pixelPos.x, m_pixelPos.y, m_dimension, m_seed);
		int stratum = permutationElement(m_sampleIndex, samplesPerPixel(), hash_);

		m_dimension++;
		return (stratum + rng.uniform()) / samplesPerPixel();
	}

	
	glm::vec2 get2D() override {
		uint32_t hash_ = hash(m_pixelPos.x, m_pixelPos.y, m_dimension, m_seed);
		int stratum = permutationElement(m_sampleIndex, samplesPerPixel(), hash_);

		m_dimension += 2;
		int x = stratum % m_xSamplesNB, y = stratum / m_xSamplesNB;
		float dx = rng.uniform();
		float dy = rng.uniform();
		//printf("%f\n", dx);
		return glm::vec2( (x + dx) / m_xSamplesNB, (y + dy) / m_ySamplesNB);
	}

	void startSample(glm::uvec2 pixelPos, int sampleIndex, int seed) override {
		m_pixelPos = pixelPos;
		m_sampleIndex = sampleIndex;
		m_dimension = 0;
		m_seed = seed;
		rng.setSeed(rand());
	}

	int samplesPerPixel() override {
		return m_xSamplesNB*m_ySamplesNB;
	}

private:
	
	int m_dimension = 0;
	int m_sampleIndex = 0;
	int m_xSamplesNB;
	int m_ySamplesNB;
};