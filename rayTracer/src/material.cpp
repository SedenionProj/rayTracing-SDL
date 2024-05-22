#include "material.h"
glm::vec3 Diffuse::getBSDF(glm::vec3 wo, glm::vec3 wi, HitInfo& rec)
{
	rec.normal;
	if (glm::dot(wi, rec.normal) < 0) {
		return glm::vec3(0);
	}
	return m_color / PI;
}