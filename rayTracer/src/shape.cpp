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

	// uv
	auto theta = std::acos(-normal.y);
	auto phi = std::atan2(-normal.z, normal.x) + PI;

	float u = phi / (2 * PI);
	float v = theta / PI;

	// rec
	rec.normal = orientNormal(normal, ray.direction);
	rec.t = root;
	rec.pos = p;
	rec.material = material;
	rec.light = light;
	rec.uv = glm::vec2(u, v);

	return true;
}

bool Scene::intersect(Ray& ray, HitInfo& rec, float tMin, float tMax) {
	return bvh->intersect(ray, rec, tMin, tMax);
}

void Scene::build() {
	bvh = std::make_unique<BVHNode>(objects, 0, objects.size());
}

void Scene::addShape(const std::shared_ptr<Shape> obj) {
	if (obj->light) {
		lights.push_back(obj->light);
	}

	objects.push_back(obj);
}

void Scene::addSky(std::shared_ptr<Sky> skyLight) {
	lights.push_back(skyLight);
	sky = skyLight;
}

constexpr float kEpsilon = 1e-8;

aiMesh* Triangle::m_mesh = nullptr;

bool Triangle::intersect(Ray& ray, HitInfo& rec, float tMin , float tMax)
{
	aiFace face = m_mesh->mFaces[m_id];
	int id0 = face.mIndices[0], id1 = face.mIndices[1], id2 = face.mIndices[2];
	aiVector3D vp0 = m_mesh->mVertices[id0], vp1 = m_mesh->mVertices[id1], vp2 = m_mesh->mVertices[id2];
	glm::vec3 v0 = glm::vec3(vp0.x, vp0.y, vp0.z);
	glm::vec3 v1 = glm::vec3(vp1.x, vp1.y, vp1.z);
	glm::vec3 v2 = glm::vec3(vp2.x, vp2.y, vp2.z);

	glm::vec3 v0v1 = v1 - v0;
	glm::vec3 v0v2 = v2 - v0;
	glm::vec3 pvec = glm::cross(ray.direction,v0v2);
	float det = glm::dot(v0v1,pvec);
#if 0
	// if the determinant is negative the triangle is backfacing
	// if the determinant is close to 0, the ray misses the triangle
	if (det < kEpsilon) return false;
#else
	// ray and triangle are parallel if det is close to 0
	if (fabs(det) < kEpsilon) return false;
#endif
	float invDet = 1 / det;

	glm::vec3 tvec = ray.origin - v0;
	float u = glm::dot(tvec,pvec) * invDet;
	if (u < 0 || u > 1) return false;

	glm::vec3 qvec = glm::cross(tvec,v0v1);
	float v = glm::dot(ray.direction,qvec) * invDet;
	if (v < 0 || u + v > 1) return false;

	float t = glm::dot(v0v2,qvec) * invDet;

	if (t < tMin || tMax < t) {
		return false;
	}

	rec.normal = orientNormal(glm::normalize(glm::cross(v0v1, v0v2)), ray.direction);
	rec.t = t*0.9;
	rec.pos = ray.at(t*0.9);
	rec.material = material;
	rec.light = light;
	rec.uv = glm::vec2(u, v);

	return true;
}
