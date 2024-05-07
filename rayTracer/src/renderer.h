#pragma once

#include <glm/glm.hpp>
#include "camera.h"
#include "shape.h"

bool sceneIntersect() {
    return 0;
}

inline glm::vec3 getRay(glm::vec3 n) {
	float a = (rand() % 10000 - 5000.f) / 10000.f;
	float b = (rand() % 10000 - 5000.f) / 10000.f;
	float c = (rand() % 10000 - 5000.f) / 10000.f;
    return n + glm::normalize(glm::vec3(a, b, c));
}

glm::vec3 render(Ray& ray, Scene& scene) {
    HitInfo rec;
    
    glm::vec3 col = glm::vec3(1);

    for (int i = 0; i < 2; i++) {
		if (!scene.intersect(ray, rec)) {
			return col*glm::vec3(0.6,0.8,0.9);
		}

        col *= 0.8f;
        ray.direction = glm::normalize(getRay(rec.normal));
        //ray.direction = glm::reflect(ray.direction, rec.normal);
        ray.origin = rec.pos;
    }

    

    return glm::vec3(0, 0, 0);
}