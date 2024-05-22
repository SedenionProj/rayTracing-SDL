#pragma once

#include <glm/glm.hpp>
#include "camera.h"
#include "shape.h"
#include "sample.h"

glm::vec3 render(Ray& ray, Scene& scene) {
    HitInfo rec;
    
    glm::vec3 col = glm::vec3(1);

    for (int i = 0; i < 5; i++) {
		if (!scene.intersect(ray, rec)) {
			return col*glm::vec3(0.6,0.8,0.9);
		}

        glm::vec3 wp = SampleUniformSphere();

        glm::vec3 fcos = rec.material->getBSDF(-ray.direction, wp, rec) * glm::abs(glm::dot(wp, rec.normal));
        if (fcos == glm::vec3(0)) {
            return glm::vec3(0);
        }

        col *= fcos*4.f*PI;
        ray.direction = wp;
        //ray.direction = glm::reflect(ray.direction, rec.normal);
        ray.origin = rec.pos;
    }

    

    return glm::vec3(0, 0, 0);
}
