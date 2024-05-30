#pragma once

#include <glm/glm.hpp>
#include "camera.h"
#include "shape.h"
#include "utils/sample.h"
#include "spectrum.h"

glm::mat3 xyz_to_rgb = glm::mat3(
    glm::vec3(3.2406, -0.9689, 0.0557),
    glm::vec3(-1.5372, 1.8758, -0.2040),
    glm::vec3(-0.4986, 0.0415, 1.0570));

glm::vec3 renderPixel(Ray& ray, Scene& scene) {
    HitInfo rec;
    
    //glm::vec3 col = glm::vec3(1);
    float col = 1;

    WaveLength lambda;

    for (int i = 0; i < 5; i++) {
		if (!scene.intersect(ray, rec)) {
            float x = X(lambda.lambda);
            float y = Y(lambda.lambda);
            float z = Z(lambda.lambda);

			return xyz_to_rgb*col* glm::vec3(x, y, z)*glm::vec3(2.);
		}

        glm::vec3 wp = SampleUniformSphere();

        float fcos = rec.material->getBSDF(-ray.direction, wp, rec, lambda) * glm::abs(glm::dot(wp, rec.normal));
        if (fcos == 0) {
            return glm::vec3(0);
        }

        
        col *= fcos*4.f*PI;

        ray.direction = wp;
        //ray.direction = glm::reflect(ray.direction, rec.normal);
        ray.origin = rec.pos;
    }



    return glm::vec3(0, 0, 0);
}
