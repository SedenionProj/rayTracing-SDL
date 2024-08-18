#pragma once
#include <iostream>
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "shape.h"

inline void processMesh(aiMesh* mesh, const aiScene* scene, Scene& rtSecne) {
	Triangle::m_mesh = mesh;

	for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		std::shared_ptr<Triangle> tri = std::make_shared<Triangle>((int)i);
		tri->material = std::make_shared<Dielectric>(glm::vec3(1));
		rtSecne.addShape(tri);
	}
}

inline void processNode(aiNode* node, const aiScene* scene, Scene& rtSecne) {
	// Process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene, rtSecne);
	}

	// Recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		processNode(node->mChildren[i], scene, rtSecne);
	}
}

extern Assimp::Importer importer;

inline bool importMesh(const std::string& pFile, Scene& rtSecne) {
	

	const aiScene* scene = importer.ReadFile(pFile,
		aiProcess_Triangulate);

	// If the import failed, report it
	if (nullptr == scene) {
		std::cout << "couldn't import scene" << "\n";
		return false;
	}

	aiNode* node = scene->mRootNode;

	processNode(node, scene, rtSecne);

	return true;
}

