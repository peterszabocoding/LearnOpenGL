#include "mgpch.h"

#include "AssetLoader.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Moongoose/Renderer/Mesh.h"
#include "Moongoose/Renderer/Texture.h"
#include "Moongoose/Renderer/Bounds.h"

#include "glm/glm.hpp"

namespace Moongoose {

	Ref<Asset> Moongoose::MeshAssetLoader::LoadAsset(const std::string& filePath)
	{
		Assimp::Importer importer;
		unsigned int importerFlags = aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_GenSmoothNormals |
			aiProcess_JoinIdenticalVertices |
			aiProcess_CalcTangentSpace;

		const aiScene* scene = importer.ReadFile(filePath, importerFlags);
		if (!scene)
		{
			LOG_CORE_ERROR("Model ({0}) failed to load: {1}", filePath, importer.GetErrorString());
			return nullptr;
		}

		aiMesh* mesh = scene->mMeshes[scene->mRootNode->mChildren[0]->mMeshes[0]];

		glm::vec3 min = glm::vec3(
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max()
		);

		glm::vec3 max = glm::vec3(
			std::numeric_limits<float>::lowest(),
			std::numeric_limits<float>::lowest(),
			std::numeric_limits<float>::lowest()
		);

		std::vector<float> vertices;
		std::vector<unsigned int> indices;

		for (size_t i = 0; i < mesh->mNumVertices; i++)
		{
			min.x = mesh->mVertices[i].x < min.x ? mesh->mVertices[i].x : min.x;
			min.y = mesh->mVertices[i].y < min.y ? mesh->mVertices[i].y : min.y;
			min.z = mesh->mVertices[i].z < min.z ? mesh->mVertices[i].z : min.z;

			max.x = mesh->mVertices[i].x > max.x ? mesh->mVertices[i].x : max.x;
			max.y = mesh->mVertices[i].y > max.y ? mesh->mVertices[i].y : max.y;
			max.z = mesh->mVertices[i].z > max.z ? mesh->mVertices[i].z : max.z;

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

		auto meshAsset = CreateRef<Mesh>(&vertices[0], &indices[0], vertices.size(), indices.size());
		meshAsset->SetModelSource(filePath);
		meshAsset->SetBounds(Bounds3(min, max));

		return meshAsset;
	}

	Ref<Asset> Moongoose::TextureAssetLoader::LoadAsset(const std::string& filePath)
	{
		int width, height, bitDepth;
		uint8_t* textureData = nullptr;
		textureData = stbi_load((filePath).c_str(), &width, &height, &bitDepth, 0);

		if (!textureData) {
			LOG_CORE_ERROR("AssetManager.cpp | Failed to find: {0}", filePath);
			return nullptr;
		}
		else {
			TextureSpecs specs;
			specs.Width = width;
			specs.Height = height;
			specs.BitDepth = bitDepth;
			specs.FileLocation = filePath;
			specs.TextureFormat = TextureFormat::RGB;

			Ref<Texture2D> texture = Texture2D::Create(specs);
			texture->loadData(textureData, width, height, bitDepth);

			stbi_image_free(textureData);

			return texture;
		}
	}

}
