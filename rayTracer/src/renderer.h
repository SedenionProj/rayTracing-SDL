#pragma once

#include <glm/glm.hpp>
#include "camera.h"
#include "shape.h"
#include "utils/sample.h"
#include "spectrum.h"

glm::vec3 renderPixel(Ray& ray, Scene& scene) {
    HitInfo rec;
    
    float col = 1;

    WaveLength lambda;

    for (int i = 0; i < 5; i++) {
		if (!scene.intersect(ray, rec)) {
            float x = X(lambda.lambda);
            float y = Y(lambda.lambda);
            float z = Z(lambda.lambda);

			return col* glm::vec3(x, y, z) * light(lambda.lambda);
		}

        glm::vec3 wp = SampleUniformSphere();

        float fcos = rec.material->getBSDF(-ray.direction, wp, rec, lambda) * glm::abs(glm::dot(wp, rec.normal));
        if (fcos == 0) {
            return glm::vec3(0);
        }

        col *= fcos*4.f*PI;

        ray.direction = wp;
        ray.origin = rec.pos;
    }



    return glm::vec3(0, 0, 0);
}