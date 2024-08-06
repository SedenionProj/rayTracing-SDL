#pragma once
#include <vector>
#include <iostream>

#include "camera.h"
#include "material.h"
#include "spectrum.h"
#include "light.h"
#include "utils/math.h"
#include "utils/constant.h"

class Material;

struct HitInfo {
	float Le(float lambda) const {
        if (!light)
            return 0;
        return light->Le(lambda);
    }
	
	float t;
	glm::vec3 pos;
	glm::vec3 normal;
	std::shared_ptr<Material> material;
	std::shared_ptr<AreaLight> light;

	glm::vec2 uv;
};

class Object {
public:
	virtual bool intersect(Ray& ray, HitInfo& rec, float tMin = 0.01f, float tMax = MAX_FLOAT ) = 0;
	AABB boundingBox;
};

class Shape : public Object {
public:
	virtual std::tuple<glm::vec3, float> sample(glm::vec2 sample, glm::vec3 pos) = 0;
public:
	std::shared_ptr<Material> material;
	std::shared_ptr<AreaLight> light;
};

class Sphere : public Shape {
public:
    Sphere(const glm::vec3& origin, const float radius);

    bool intersect(Ray& ray, HitInfo& rec, float tMin = 0.01f, float tMax = MAX_FLOAT) override;
    
	std::tuple<glm::vec3, float> sample(glm::vec2 sample, glm::vec3 pos) {
		// sample a point outside the sphere
		float sinThetaMax = r / glm::distance(pos, origin);
		float sin2ThetaMax = sinThetaMax * sinThetaMax;
		float cosThetaMax = glm::sqrt(1 - sin2ThetaMax);
		float oneMinusCosThetaMax = 1 - cosThetaMax;

		float cosTheta = (cosThetaMax - 1) * sample.x + 1;
		float sin2Theta = 1 - cosTheta * cosTheta;

		float cosAlpha = sin2Theta / sinThetaMax + 
			cosTheta * glm::sqrt(1 - sin2Theta / (sinThetaMax * sinThetaMax) );
		float sinAlpha = glm::sqrt(1 - cosAlpha* cosAlpha);

		float phi = sample.y * 2 * PI;
		glm::vec3 w = sphericalDirection(sinAlpha, cosAlpha, phi);
		auto cs = CoordinateSystem(glm::normalize(origin-pos));
		glm::vec3 n = cs.transform(-w);
		glm::vec3 p = origin + r * n*1.01f;
		return { p, 1.f / (2.f * PI * oneMinusCosThetaMax) };
	}

    float r;
    glm::vec3 origin;
};

class BVHNode : public Object {
public:
	BVHNode(std::vector<std::shared_ptr<Object>>& objects, size_t begin, size_t end) {
		if (end - begin == 1) {
			left = objects[begin];
			right = objects[begin];
		}
		else if (end - begin == 2) {
			left = objects[begin];
			right = objects[begin + 1];
		}
		else {
			int axis = randInt(0, 2);
			auto comparator = axis == 0 ? xAxisComp : axis == 1 ? yAxisComp : zAxisComp;
			size_t middle = (begin + end) / 2;
			std::sort(objects.begin() + begin, objects.begin() + end, comparator);
			left = std::make_shared<BVHNode>(objects, begin, middle);
			right = std::make_shared<BVHNode>(objects, middle, end);
		}
		boundingBox = AABB(left->boundingBox, right->boundingBox);
	}

	bool intersect(Ray& ray, HitInfo& rec, float tMin = 0.01f, float tMax = MAX_FLOAT) {
		if (!boundingBox.intersect(ray, tMin, tMax))
			return false;
			
		bool l = left->intersect(ray, rec, tMin, tMax);
		bool r = right->intersect(ray, rec, tMin, l ? rec.t : tMax);

		return l || r;
	}

private:
	static bool xAxisComp(const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) {
		float aCenter = (a->boundingBox.bMin.x + a->boundingBox.bMax.x);
		float bCenter = (b->boundingBox.bMin.x + b->boundingBox.bMax.x);
		return aCenter < bCenter;
	}

	static bool yAxisComp(const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) {
		float aCenter = (a->boundingBox.bMin.y + a->boundingBox.bMax.y);
		float bCenter = (b->boundingBox.bMin.y + b->boundingBox.bMax.y);
		return aCenter < bCenter;
	}

	static bool zAxisComp(const std::shared_ptr<Object>& a, const std::shared_ptr<Object>& b) {
		float aCenter = (a->boundingBox.bMin.z + a->boundingBox.bMax.z);
		float bCenter = (b->boundingBox.bMin.z + b->boundingBox.bMax.z);
		return aCenter < bCenter;
	}

	std::shared_ptr<Object> left;
	std::shared_ptr<Object> right;
};

class Scene : public Object {
public:
	bool intersect(Ray& ray, HitInfo& rec, float tMin = 0.01f, float tMax = MAX_FLOAT);
	void build();
	void addShape(const std::shared_ptr<Shape> obj);
	void addSky(const std::shared_ptr<Sky> skyLight);
	std::shared_ptr<Light> sampleLight(float sample) {
		return lights[glm::min(glm::floor(lights.size() * sample), lights.size()-1.f)];
	}

	std::shared_ptr<Sky> sky;
	std::vector<std::shared_ptr<Object>> objects;
	std::unique_ptr<BVHNode> bvh;

private:
	std::vector<std::shared_ptr<Light>> lights;
};