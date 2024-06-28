#pragma once
#include <vector>
#include <iostream>

#include "camera.h"
#include "material.h"
#include "spectrum.h"
#include "light.h"
#include "utils/math.h"

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
	std::shared_ptr<Material> material;
	std::shared_ptr<AreaLight> light;
    AABB boundingBox;
};

class Sphere : public Object {
public:
    Sphere(const glm::vec3& origin, const float radius);

    bool intersect(Ray& ray, HitInfo& rec) override;
    
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

	bool intersect(Ray& ray, HitInfo& rec) { 
		if (!boundingBox.intersect(ray, 0, 1000))
			return false;
			
		return left->intersect(ray, rec) || right->intersect(ray, rec);
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
	bool intersect(Ray& ray, HitInfo& rec);

	void build();

	Sky* sky;
	std::vector<std::shared_ptr<Object>> objects;

	std::unique_ptr<BVHNode> bvh;
};