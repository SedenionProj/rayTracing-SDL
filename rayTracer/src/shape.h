#pragma once
#include <vector>
#include <iostream>

#include "camera.h"
#include "material.h"
#include "spectrum.h"

class Material;

struct HitInfo {
	float t;
	glm::vec3 pos;
	glm::vec3 normal;
	std::shared_ptr<Material> material;
};

class Object
{
public:
	virtual bool intersect(Ray& ray, HitInfo& rec) = 0;
public:
    std::shared_ptr<Material> material;
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

class Sky {
public:
    Sky(float T, float power) :
        m_spec(new BlackBodySpectrum(T)), m_power(power) {}

    float Le(float lambda) {
        return m_power * (*m_spec)(lambda);
    }
private:
    float m_power;
    Spectrum* m_spec;
};

class Scene {
public:
    bool intersect(Ray& ray, HitInfo& rec);

    Sky* sky;
    std::vector<Object*> objects;
};