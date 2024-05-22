#include "shape.h"
#include "material.h"

bool Sphere::intersect(Ray& ray, HitInfo& rec)
{
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
	if (root < 0.01 || MAX_FLOAT < root) {
		root = (-half_b + sqrtd) / a;
		if (root < 0.01 || MAX_FLOAT < root) {
			return false;
		}
	}
	glm::vec3 p = ray.at(root);
	glm::vec3 normal = (p - origin) / r;

	rec.normal = orientNormal(normal, ray.direction);
	rec.t = root;
	rec.pos = p;
	
	return true;
}

bool Scene::intersect(Ray& ray, HitInfo& rec)
{
	bool hit = false;
	HitInfo closestHit;
	float closestT = MAX_FLOAT;
	for (Object* obj : objects) {
		if (obj->intersect(ray, closestHit)) {
			hit = true;
			if (closestHit.t < closestT) {
				closestT = closestHit.t;
				rec = closestHit;
				rec.material = obj->material;
			}
		};
	}
	return hit;
}
