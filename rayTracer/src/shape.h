#pragma once
#include <vector>

#include "camera.h"

struct HitInfo {
    float t;
    glm::vec3 pos;
    glm::vec3 normal;
};

class Object
{
public:
	virtual bool intersect(Ray& ray, HitInfo& rec) = 0;
};

inline glm::vec3 orientNormal(glm::vec3& normal, glm::vec3& direction) {
    if (glm::dot(normal, direction) < 0.f)
        return normal;
    return    -normal;
}

class Sphere : public Object {
public:
    bool intersect(Ray& ray, HitInfo& rec) override {
        glm::vec3 oc = ray.origin - origin;
        float a = glm::dot(ray.direction, ray.direction);
        float half_b = glm::dot(oc, ray.direction);
        float c = dot(oc, oc) - r * r;
        float discriminant = half_b * half_b - a * c;
        if (discriminant < 0.0) {
            return false;
        }
        float sqrtd = sqrt(discriminant);

        float root = (-half_b - sqrtd) / a;
        if (root < 0.01 || 999999 < root) {
            root = (-half_b + sqrtd) / a;
            if (root < 0.01 || 999999 < root) {
                return false;
            }
        }
        glm::vec3 p = ray.at(root);
        glm::vec3 normal = (p - origin) / r;

        rec.t = root;
        rec.pos = p;
        rec.normal = orientNormal(normal, ray.direction);

        return true;
    }
    
    float r;
    glm::vec3 origin;

};


class Scene {
public:
    bool intersect(Ray& ray, HitInfo& rec) {
        bool hit = false;
        HitInfo closestHit;
        float closestT = 999999;
        for (Object* obj : objects) {
            if (obj->intersect(ray, closestHit)) {
                hit = true;
                if (closestHit.t < closestT) {
					closestT = rec.t;
					rec = closestHit;
                }
            };
        }
        return hit;
    }

    std::vector<Object*> objects;
};