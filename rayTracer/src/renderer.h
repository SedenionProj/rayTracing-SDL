#pragma once

#include <glm/glm.hpp>
#include "camera.h"
#include "shape.h"
#include "utils/sample.h"
#include "spectrum.h"
#include "utils/sample.h"
#include "camera.h"

// no dynamic dispatch as vtables can slow down the program
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

				//IndependantSampler sampler(4);
				StratifiedSampler sampler(2,2);

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

		float col = naivePathTracerRecursive(rec, lambda, ray, sampler, 5);

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

		float fcos = rec.material->getBSDF(-ray.direction, wp, rec, lambda) * glm::abs(glm::dot(wp, rec.normal));

		if (fcos == 0) {
			return Le;
		}

		ray.direction = wp;
		ray.origin = rec.pos;

		return Le + fcos * naivePathTracerRecursive(rec, lambda, ray, sampler, depth - 1) * 4.f * PI;
	}

private:
	Scene& scene;
	Camera& camera;

	std::vector<float> verticalIT, horizontalIT;
};