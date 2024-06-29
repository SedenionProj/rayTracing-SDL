#include "shape.h"
#include "material.h"

Sphere::Sphere(const glm::vec3& origin, const float radius)
	: origin(origin), r(radius){
	boundingBox = AABB(origin - radius, origin + radius);
}

bool Sphere::intersect(Ray& ray, HitInfo& rec, float tMin, float tMax) {
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
	if (root < tMin || tMax < root) {
		root = (-half_b + sqrtd) / a;
		if (root < tMin || tMax < root) {
			return false;
		}
	}
	glm::vec3 p = ray.at(root);
	glm::vec3 normal = (p - origin) / r;

	rec.normal = orientNormal(normal, ray.direction);
	rec.t = root;
	rec.pos = p;
	rec.material = material;
	rec.light = light;

	return true;
}

bool Scene::intersect(Ray& ray, HitInfo& rec, float tMin, float tMax) {
	return bvh->intersect(ray, rec, tMin, tMax);
}

void Scene::build() {
	bvh = std::make_unique<BVHNode>(objects, 0, objects.size());
}
