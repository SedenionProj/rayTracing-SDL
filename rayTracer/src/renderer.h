#pragma once

#include <glm/glm.hpp>
#include "camera.h"
#include "shape.h"

bool sceneIntersect() {
    return 0;
}

glm::vec3 render(Ray& ray, Scene& scene) {
    HitInfo rec;

    if (scene.intersect(ray, rec)) {
        return (rec.normal+1.f)*0.5f;
    }
    return glm::vec3(0, 0, 0);
}