
#include "mgpch.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Moongoose/Renderer/Mesh.h"
#include "AssetManager.h"

namespace Moongoose {

	Mesh* AssetManager::LoadMesh(std::string meshPath)
	{
		Assimp::Importer importer;
		unsigned int importerFlags = aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_GenSmoothNormals |
			aiProcess_JoinIdenticalVertices |
			aiProcess_CalcTangentSpace;

		const aiScene* scene = importer.ReadFile(meshPath, importerFlags);
		if (!scene)
		{
			LOG_CORE_ERROR("Model ({0}) failed to load: {1}", meshPath, importer.GetErrorString());
			return nullptr;
		}
		
		return LoadMesh(scene->mMeshes[scene->mRootNode->mChildren[0]->mMeshes[0]], scene);
	}

	Mesh* AssetManager::LoadMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<GLfloat> vertices;
		std::vector<unsigned int> indices;

		for (size_t i = 0; i < mesh->mNumVertices; i++)
		{
			// Position
			vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y , mesh->mVertices[i].z });

			// UV
			auto vertexX = mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][i].x : 0;
			auto vertexY = mesh->mTextureCoords[0] ? mesh->mTextureCoords[0][i].y : 0;
			vertices.insert(vertices.end(), { vertexX, vertexY });

			// Normal
			vertices.insert(vertices.end(), { mesh->mNormals[i].x, mesh->mNormals[i].y , mesh->mNormals[i].z });

			// Tangent and Bitangent
			vertices.insert(vertices.end(), { mesh->mTangents[i].x, mesh->mTangents[i].y , mesh->mTangents[i].z });
			vertices.insert(vertices.end(), { mesh->mBitangents[i].x, mesh->mBitangents[i].y , mesh->mBitangents[i].z });
		}

		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace face = mesh->mFaces[i];
			for (size_t j = 0; j < face.mNumIndices; j++)
			{
				indices.push_back(face.mIndices[j]);
			}
		}

		return new Mesh(&vertices[0], &indices[0], vertices.size(), indices.size());
	}
	void AssetManager::LoadNode(aiNode* node, const aiScene* scene)
	{
		for (size_t i = 0; i < node->mNumMeshes; i++) {
			LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);
		}

		for (size_t i = 0; i < node->mNumChildren; i++) {
			LoadNode(node->mChildren[i], scene);
		}
	}
}
