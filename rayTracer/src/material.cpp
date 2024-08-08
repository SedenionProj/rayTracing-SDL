#include "material.h"
#include "spectrum.h"

float Diffuse::BSDF_f(glm::vec3 wo, glm::vec3 wi, HitInfo& rec,WaveLength lambda)
{
	if (glm::dot(wi, rec.normal) < 0) {
		return 0;
	}
	return m_texture->eval(lambda, rec.uv) / PI;
}

BSDFSample Diffuse::SampleBSDF(const glm::vec3& woTr, const glm::vec3& n, Sampler& sampler, HitInfo& rec, WaveLength lambda) {
	CoordinateSystem cs(n);
	glm::vec3 wo = cs.invTransform(woTr);
	glm::vec3 wi = sampleRandomCosineDirection(sampler);
	if (wo.z < 0) wi.z *= -1;
	float pdf = glm::abs(wi.z) / PI;
	wi = cs.transform(wi);	

	return { m_texture->eval(lambda, rec.uv) / PI,wi, pdf };
}
