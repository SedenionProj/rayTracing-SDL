#pragma once
#include <glm/glm.hpp>
#include <complex>
#include "../camera.h"

inline float lerp(float t, float min, float max) {
	return (1.f - t) * min + t * max;
}

inline glm::vec3 orientNormal(glm::vec3& normal, glm::vec3& direction) {
	return  glm::dot(normal, direction) < 0.f ? normal: -normal;
}

inline glm::vec3 sphericalDirection(float sinTheta, float cosTheta, float phi) {
	return glm::vec3(sinTheta * std::cos(phi),
					 sinTheta* std::sin(phi),
				     cosTheta);
}

inline float fresnelComplex(float cosTheta_i, std::complex<float> eta) {
	using Complex = std::complex<float>;
	cosTheta_i = glm::clamp(cosTheta_i, 0.f, 1.f);
	float sin2Theta_i = 1 - cosTheta_i* cosTheta_i;
	Complex sin2Theta_t = sin2Theta_i / (eta* eta);
	Complex cosTheta_t = std::sqrt(1.f - sin2Theta_t);
	Complex r_parl = (eta* cosTheta_i - cosTheta_t) / (eta* cosTheta_i + cosTheta_t);
	Complex r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);
	return (std::norm(r_parl) + std::norm(r_perp)) / 2.f;
}

inline float fresnelDielectric(float cosTheta_i, float eta) {
	cosTheta_i = glm::clamp(cosTheta_i, -1.f, 1.f);
	if (cosTheta_i < 0) {
		eta = 1 / eta;
		cosTheta_i = -cosTheta_i;
	} 

	float sin2Theta_i = 1 - cosTheta_i* cosTheta_i;
	float sin2Theta_t = sin2Theta_i / (eta* eta);
	if (sin2Theta_t >= 1)
		return 1.f;
	float cosTheta_t = glm::sqrt(1.f - sin2Theta_t);


	float r_parl = (eta* cosTheta_i - cosTheta_t) / (eta* cosTheta_i + cosTheta_t);
	float r_perp = (cosTheta_i - eta * cosTheta_t) / (cosTheta_i + eta * cosTheta_t);


	return (r_parl* r_parl + r_perp* r_perp) / 2;
}

inline glm::mat3 xyz_to_rgb = glm::mat3(
	glm::vec3(3.2406, -0.9689, 0.0557),
	glm::vec3(-1.5372, 1.8758, -0.2040),
	glm::vec3(-0.4986, 0.0415, 1.0570));

inline bool refract(glm::vec3 wi, glm::vec3 n, float eta, float* etap, glm::vec3* wt) {
	float cosTheta_i = glm::dot(n, wi);
	// Potentially flip interface orientation for Snell's law
	if (cosTheta_i < 0) {
		eta = 1 / eta;
		cosTheta_i = -cosTheta_i;
		n = -n;
	}

	// Compute $\cos\,\theta_\roman{t}$ using Snell's law
	float sin2Theta_i = glm::max(0.f, 1 - (cosTheta_i* cosTheta_i));
	float sin2Theta_t = sin2Theta_i / (eta* eta);
	// Handle total internal reflection case
	if (sin2Theta_t >= 1)
		return false;

	float cosTheta_t = glm::sqrt(1 - sin2Theta_t);

	*wt =  - wi / eta + (cosTheta_i / eta - cosTheta_t) * n;
	// Provide relative IOR along ray to caller
	if (etap)
		*etap = eta;

	return true;
}

inline float PowerHeuristic(int nf, float fPdf, int ng, float gPdf) {
	float f = nf * fPdf, g = ng * gPdf;
	return f*f / ((f*f) + (g*g));
}

class AABB {
public:
	AABB() : bMin{}, bMax{} {}

	AABB(const AABB& a, const AABB& b)
		: bMin(glm::vec3(std::min(a.bMin.x, b.bMin.x), std::min(a.bMin.y, b.bMin.y), std::min(a.bMin.z, b.bMin.z))),
		  bMax(glm::vec3(std::max(a.bMax.x, b.bMax.x), std::max(a.bMax.y, b.bMax.y), std::max(a.bMax.z, b.bMax.z))) {};

	AABB(const glm::vec3& a, const glm::vec3& b)
		: bMin(glm::vec3(std::min(a.x, b.x), std::min(a.y, b.y), std::min(a.z, b.z))),
		  bMax(glm::vec3(std::max(a.x, b.x), std::max(a.y, b.y), std::max(a.z, b.z))) {};

	bool intersect(const Ray& ray, float tmin, float tmax) const {
		for (int a = 0; a < 3; ++a) {
			float invD = 1.0f / ray.direction[a];
			float t0 = (bMin[a] - ray.origin[a]) * invD;
			float t1 = (bMax[a] - ray.origin[a]) * invD;
			if (invD < 0.0f) {
				std::swap(t0, t1);
			}

			tmin = std::max(t0, tmin);
			tmax = std::min(t1, tmax);

			if (tmax <= tmin)
				return false;
		}

		return true;
	}

	glm::vec3 bMin;
	glm::vec3 bMax;
};

class CoordinateSystem {
public:
	CoordinateSystem(const glm::vec3& n) {
		w = glm::normalize(n);
		glm::vec3 a = (glm::abs(w.x) > 0.9) ? glm::vec3(0, 1, 0) : glm::vec3(1, 0, 0);
		v = glm::normalize(glm::cross(w, a));
		u = glm::cross(w, v);
	}

	glm::vec3 transform(const glm::vec3& vec) {
		return vec.x * u + vec.y * v + vec.z * w;
	}

	glm::vec3 invTransform(const glm::vec3& vec) {
		return glm::vec3(glm::dot(vec, u), glm::dot(vec, v), glm::dot(vec, w));
	}
private:
	glm::vec3 u,v, w;
};

