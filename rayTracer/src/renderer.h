#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "camera.h"
#include "shape.h"
#include "utils/sample.h"
#include "spectrum.h"
#include "utils/sample.h"
#include "camera.h"



class Renderer {
public:
	Renderer(Scene& scene, Camera& camera) :
		scene(scene), camera(camera) {
		verticalIT.resize(camera.film.m_resolution.x);
		horizontalIT.resize(camera.film.m_resolution.y);
		for (int i = 0; i < camera.film.m_resolution.x; i++)
			verticalIT[i] = i;
		for (int i = 0; i < camera.film.m_resolution.y; i++)
			horizontalIT[i] = i;
	}

	void render(bool isMoving, SDL_Surface* gSurface) {

		camera.film.totalSamplesNB += 4;

		std::for_each(std::execution::par, verticalIT.begin(), verticalIT.end(), [&](int y) {
			std::for_each(std::execution::par, horizontalIT.begin(), horizontalIT.end(), [&, y](int x) {

				glm::uvec2 pixelPos = glm::uvec2(x, y);

				IndependantSampler sampler(4);
				//StratifiedSampler sampler(2,2);

				for (int i = 0; i < sampler.samplesPerPixel(); i++) {
					sampler.startSample(pixelPos, i, camera.film.totalSamplesNB);
					if (isMoving)
						camera.film.setSample(pixelPos, renderPixel(sampler, pixelPos));
					else
						camera.film.addSample(pixelPos, renderPixel(sampler, pixelPos));
				}

				glm::vec3 rgb_color = xyz_to_rgb * (camera.film.getSample(pixelPos) / (float)camera.film.totalSamplesNB);
				rgb_color = glm::pow(rgb_color, glm::vec3(1.0 / 2.2));
				Uint32* pixel = (Uint32*)((Uint8*)gSurface->pixels + y * gSurface->pitch + x * sizeof(Uint32));
				*pixel = SDL_MapRGB(gSurface->format,
					glm::clamp(rgb_color.x, 0.f, 1.f) * 255.f,
					glm::clamp(rgb_color.y, 0.f, 1.f) * 255.f,
					glm::clamp(rgb_color.z, 0.f, 1.f) * 255.f);
				});
			});
	}

private:

	glm::vec3 renderPixel(Sampler& sampler, const glm::vec2& pixelPos) {
		Ray ray = camera.getRay(pixelPos, sampler);
		WaveLength lambda(sampler);
		HitInfo rec{};

		float col = naivePathTracerRecursive(rec, lambda, ray, sampler, 1);

		float x = X(lambda);
		float y = Y(lambda);
		float z = Z(lambda);

		return glm::vec3(x, y, z) * col;
	}

	float naivePathTracerRecursive(HitInfo& rec, const WaveLength& lambda, Ray& ray, Sampler& sampler, int depth) {

		if (!scene.intersect(ray, rec)) {
			return scene.sky->Le(lambda);
		}

		float Le = rec.Le(lambda);

		if (depth == 0) {
			return Le;
		}

		glm::vec3 wp = SampleUniformSphere(sampler.get2D());

		float fcos = rec.material->BSDF_f(-ray.direction, wp, rec, lambda) * glm::abs(glm::dot(wp, rec.normal));

		if (fcos == 0) {
			return Le;
		}

		ray.direction = wp;
		ray.origin = rec.pos;

		return Le + fcos * naivePathTracerRecursive(rec, lambda, ray, sampler, depth - 1) * 4.f * PI;
	}

	float simplePathTracer(HitInfo& rec, const WaveLength& lambda, Ray& ray, Sampler& sampler, int depth) {
		float L = 0;
		float beta = 1;
		bool test = true; // specular bounce

		while (true) {
			if (!scene.intersect(ray, rec)) {
				if (test)
					L += beta * scene.sky->Le(lambda);
				break;
			}

			if (test) {
				L += beta * rec.Le(lambda);
			}

			if (depth-- == 0) {
				break;
			}

			glm::vec3 wo = -ray.direction;
			auto sampledLight = scene.sampleLight(sampler.get1D());
			LightSample ls = sampledLight->sampleLi(sampler.get2D(), rec.pos, lambda);
			if (glm::length2(ls.pos - rec.pos) > 0.00001 && ls.L && ls.pdf > 0.f) {
				float f = rec.material->BSDF_f(wo, ls.wi, rec, lambda) * glm::abs(glm::dot(ls.wi, rec.normal));
				if (f != 0 && unoccluded(rec.pos, ls.pos)) {
					L += beta * f * ls.L / (ls.pdf * scene.pmf());
				}
			}

			BSDFSample bs = rec.material->SampleBSDF(wo, rec.normal, sampler, rec, lambda);
			if (!bs.f || bs.pdf == 0 || bs.wi.z == 0) {
				break;
			}
			beta *= bs.f * glm::abs(glm::dot(bs.wi, rec.normal)) / bs.pdf;
			ray.origin = rec.pos;
			ray.direction = bs.wi;

			test = false;
		}

		return L;
	}

	float tester(HitInfo& rec, const WaveLength& lambda, Ray& ray, Sampler& sampler, int depth) {
		if (!scene.intersect(ray, rec)) {
			return scene.sky->Le(lambda);
		}

		float Le = rec.Le(lambda);

		if (depth == 0) {
			return Le;
		}

		BSDFSample bs = rec.material->SampleBSDF(-ray.direction, rec.normal, sampler, rec, lambda);

		if (!bs.f || bs.pdf == 0 || bs.wi.z == 0) {
			return Le;
		}

		ray.origin = rec.pos;
		ray.direction = bs.wi;


		float fcos = bs.f * glm::abs(glm::dot(bs.wi, rec.normal));

		if (fcos == 0) {
			return Le;
		}


		return Le + fcos * tester(rec, lambda, ray, sampler, depth - 1) / bs.pdf;
	}

private:
	inline bool unoccluded(glm::vec3 p1, glm::vec3 p2) {
		Ray ray(p1, glm::normalize(p2-p1));
		HitInfo rec;
		if(!scene.intersect(ray, rec))
			return true;
		if (glm::length2(p1 - rec.pos) < (glm::length2(p2 - p1)-0.01))
			return false;
		return true;
	}

private:
	Scene& scene;
	Camera& camera;

	std::vector<float> verticalIT, horizontalIT;
};