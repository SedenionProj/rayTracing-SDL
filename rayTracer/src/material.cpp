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


BSDFSample Specular::SampleBSDF(const glm::vec3& woTr, const glm::vec3& n, Sampler& sampler, HitInfo& rec, WaveLength lambda)
{
	CoordinateSystem cs(n);
	glm::vec3 wo = cs.invTransform(woTr);

	glm::vec3 wi = glm::vec3(-wo.x, -wo.y, wo.z);

	// todo custom eta and k and roughness
	float refl = glm::clamp(m_texture->eval(lambda, rec.uv),0.f,.9999f);
	float eta = 1;
	float k = 2 * glm::sqrt(refl) / glm::sqrt(glm::max(0.f,1 - refl));

	float f = fresnelComplex( glm::abs(wi.z), std::complex<float>(eta, k) ) / glm::abs(wi.z);

	wi = cs.transform(wi);
	return { f ,wi, 1.f };
}

BSDFSample Dielectric::SampleBSDF(const glm::vec3& woTr, const glm::vec3& n, Sampler& sampler, HitInfo& rec, WaveLength lambda)
{
	CoordinateSystem cs(n);
	glm::vec3 wo = cs.invTransform(woTr);

	float eta = 1.5;
	float R = fresnelDielectric(wo.z, eta);
	
	float T = 1 - R;
	float pr = R, pt = T;

	//std::cout << T << "\n";

	if (sampler.get1D() < pr / (pr + pt) ) {
		// Sample perfect specular dielectric BRDF
		glm::vec3 wi = glm::vec3(-wo.x, -wo.y, wo.z);
		float fr = (R / glm::abs(wi.z));

		wi = cs.transform(wi);
		return { fr, wi, pr / (pr + pt) };
	}
	else {
		float etap;

		glm::vec3 wi;
		bool canRefract = refract(wo, glm::vec3(0, 0, 1), -eta, &etap, &wi);

		float ft = (T / glm::abs(wi.z));
		//ft /= (eta * eta);

		wi = cs.transform(wi);
		return { ft, wi, pt / (pr + pt) };
	}
}