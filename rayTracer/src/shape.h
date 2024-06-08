#pragma once
#include <vector>
#include <iostream>

#include "camera.h"
#include "material.h"
#include "spectrum.h"
#include "light.h"

class Material;

struct HitInfo {
	float t;
	glm::vec3 pos;
	glm::vec3 normal;
	std::shared_ptr<Material> material;
    std::shared_ptr<AreaLight> light;
    float Le(float lambda) {
        if (!light)
            return 0;
        return light->Le(lambda);
    }
};

class Object
{
public:
	virtual bool intersect(Ray& ray, HitInfo& rec) = 0;
public:
    std::shared_ptr<Material> material;
    std::shared_ptr<AreaLight> light;
};

inline glm::vec3 orientNormal(glm::vec3& normal, glm::vec3& direction) {
    if (glm::dot(normal, direction) < 0.f)
        return normal;
    return    -normal;
}

class Sphere : public Object {
public:
    bool intersect(Ray& ray, HitInfo& rec) override;
    
    float r;
    glm::vec3 origin;

};

class Scene {
public:
    bool intersect(Ray& ray, HitInfo& rec);

    Sky* sky;
    std::vector<Object*> objects;
};