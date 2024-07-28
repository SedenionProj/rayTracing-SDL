#pragma once
#include "utils/constant.h"
#include "shape.h"
#include "spectrum.h"
#include "texture.h"

struct HitInfo;

class Material {
public:
	Material() {};
	virtual float getBSDF(glm::vec3 wo, glm::vec3 wi, HitInfo& rec, WaveLength lambda) = 0;
};

class Diffuse : public Material {
public:
	Diffuse(const glm::vec3& color)
		: m_texture(std::make_unique<ColorTexture>(color)) {}

	Diffuse(const char* path)
		: m_texture(std::make_unique<ImageTexture>(path)) {}
	
	float getBSDF(glm::vec3 wo, glm::vec3 wi, HitInfo& rec, WaveLength lambda) override;

	std::unique_ptr<Texture> m_texture;
};