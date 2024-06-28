#pragma once

#include <glm/glm.hpp>
#include "camera.h"
#include "shape.h"
#include "utils/sample.h"
#include "spectrum.h"

float naivePathTracer(HitInfo& rec, Scene& scene, const WaveLength& lambda, Ray& ray) {
	float col = 1;
	float Le = 0;

	for (int i = 0; i < 5; i++) {
		if (!scene.intersect(ray, rec)) {
			return Le + col * scene.sky->Le(lambda.lambda); //light(lambda.lambda);
		}

		glm::vec3 wp = SampleUniformSphere();

		Le = rec.Le(lambda.lambda);

		float fcos = rec.material->getBSDF(-ray.direction, wp, rec, lambda) * glm::abs(glm::dot(wp, rec.normal));
		if (fcos == 0) {
			return col*Le;
		}

		col *= Le + fcos * 4.f * PI;

		ray.direction = wp;
		ray.origin = rec.pos;
	}

	return col * Le;
}

float naivePathTracerRecursive(HitInfo& rec, Scene& scene, const WaveLength& lambda, Ray& ray, int depth) {

		if (!scene.intersect(ray, rec)) {
 			return scene.sky->Le(lambda.lambda);
		}

		float Le = rec.Le(lambda.lambda);

		if (depth == 0) {
			return Le;
		}

		glm::vec3 wp = SampleUniformSphere();

		float fcos = rec.material->getBSDF(-ray.direction, wp, rec, lambda) * glm::abs(glm::dot(wp, rec.normal));

		if (fcos == 0) {
			return Le;
		}

		ray.direction = wp;
		ray.origin = rec.pos;

		return Le + fcos * naivePathTracerRecursive(rec, scene, lambda, ray, depth - 1) * 4.f * PI;
}

glm::vec3 renderPixel(Ray& ray,Scene& scene) {
    HitInfo rec;

    WaveLength lambda;
	float col = naivePathTracerRecursive(rec, scene, lambda, ray,5);

	float x = X(lambda.lambda);
	float y = Y(lambda.lambda);
	float z = Z(lambda.lambda);

	return glm::vec3(x, y, z) * col;
}

