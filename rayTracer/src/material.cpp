#include "material.h"
#include "spectrum.h"

float Diffuse::getBSDF(glm::vec3 wo, glm::vec3 wi, HitInfo& rec,WaveLength lambda)
{
	if (glm::dot(wi, rec.normal) < 0) {
		return 0;
	}
	return (*m_spectrum)(lambda) / PI;
}