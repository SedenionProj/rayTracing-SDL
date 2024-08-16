#pragma once
#include "utils/constant.h"
#include "utils/sample.h"
#include "shape.h"
#include "spectrum.h"
#include "texture.h"

struct HitInfo;

class BSDFSample {
public:
	float f;
	glm::vec3 wi;
	float pdf = 0;
};

class Material {
public:
	Material() {};
	virtual float BSDF_f(glm::vec3 wo, glm::vec3 wi, HitInfo& rec, WaveLength lambda) = 0;
	virtual BSDFSample SampleBSDF(const glm::vec3& woTr, const glm::vec3& n, Sampler& sampler, HitInfo& rec, WaveLength lambda) = 0;
	virtual float pdfBSDF(glm::vec3 woTr, glm::vec3 wiTr, glm::vec3 n) = 0;
	virtual bool isBSDFSpecular() = 0;
};

class Diffuse : public Material {
public:
	Diffuse(const glm::vec3& color)
		: m_texture(std::make_unique<ColorTexture>(color)) {}
	Diffuse(const char* path)
		: m_texture(std::make_unique<ImageTexture>(path)) {}

	float BSDF_f(glm::vec3 wo, glm::vec3 wi, HitInfo& rec, WaveLength lambda) override;
	BSDFSample SampleBSDF(const glm::vec3& woTr, const glm::vec3& n, Sampler& sampler, HitInfo& rec, WaveLength lambda) override;
	float pdfBSDF(glm::vec3 woTr, glm::vec3 wi, glm::vec3 n) override;
	bool isBSDFSpecular() override { return false; }
private:
	std::unique_ptr<Texture> m_texture;
};

class Specular : public Material {
public:
	Specular(const glm::vec3& color)
		: m_texture(std::make_unique<ColorTexture>(color)) {}
	Specular(const char* path)
		: m_texture(std::make_unique<ImageTexture>(path)) {}

	float BSDF_f(glm::vec3 wo, glm::vec3 wi, HitInfo& rec, WaveLength lambda) override { return 0.f; }
	BSDFSample SampleBSDF(const glm::vec3& woTr, const glm::vec3& n, Sampler& sampler, HitInfo& rec, WaveLength lambda) override;
	float pdfBSDF(glm::vec3 woTr, glm::vec3 wi, glm::vec3 n) override;
	bool isBSDFSpecular() override { return true; }
private:
	std::unique_ptr<Texture> m_texture;
};

class Dielectric : public Material {
public:
	Dielectric(const glm::vec3& color)
		: m_texture(std::make_unique<ColorTexture>(color)) {}
	Dielectric(const char* path)
		: m_texture(std::make_unique<ImageTexture>(path)) {}

	float BSDF_f(glm::vec3 wo, glm::vec3 wi, HitInfo& rec, WaveLength lambda) override { return 0.f;}
	BSDFSample SampleBSDF(const glm::vec3& woTr, const glm::vec3& n, Sampler& sampler, HitInfo& rec, WaveLength lambda) override;
	float pdfBSDF(glm::vec3 woTr, glm::vec3 wi, glm::vec3 n) override;
	bool isBSDFSpecular() override { return true; }
private:
	std::unique_ptr<Texture> m_texture;
};