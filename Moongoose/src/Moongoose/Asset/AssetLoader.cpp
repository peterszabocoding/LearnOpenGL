#include "mgpch.h"

#include "AssetLoader.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Moongoose/Renderer/Mesh.h"
#include "Moongoose/Renderer/Texture.h"
#include "Moongoose/Renderer/Material.h"
#include "Moongoose/Renderer/Bounds.h"
#include "Moongoose/Util/FileSystem.h"

#include "glm/glm.hpp"

#include <nlohmann/json.hpp>
#include <fstream>

namespace Moongoose {

	constexpr unsigned int MESH_IMPORT_FLAGS = aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_CalcTangentSpace;

	constexpr float MAX_FLOAT = std::numeric_limits<float>::max();
	constexpr float MIN_FLOAT = std::numeric_limits<float>::lowest();

	namespace Utils {

		static inline void LoadSubmesh(aiMesh* mesh, Ref<Mesh>& meshAsset, glm::vec3& boundsMin, glm::vec3& boundsMax)
		{
			std::vector<float> vertices;
			std::vector<unsigned int> indices;
			for (size_t i = 0; i < mesh->mNumVertices; i++)
			{
				boundsMin.x = mesh->mVertices[i].x < boundsMin.x ? mesh->mVertices[i].x : boundsMin.x;
				boundsMin.y = mesh->mVertices[i].y < boundsMin.y ? mesh->mVertices[i].y : boundsMin.y;
				boundsMin.z = mesh->mVertices[i].z < boundsMin.z ? mesh->mVertices[i].z : boundsMin.z;

				boundsMax.x = mesh->mVertices[i].x > boundsMax.x ? mesh->mVertices[i].x : boundsMax.x;
				boundsMax.y = mesh->mVertices[i].y > boundsMax.y ? mesh->mVertices[i].y : boundsMax.y;
				boundsMax.z = mesh->mVertices[i].z > boundsMax.z ? mesh->mVertices[i].z : boundsMax.z;

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
				for (size_t j = 0; j < mesh->mFaces[i].mNumIndices; j++) {
					indices.push_back(mesh->mFaces[i].mIndices[j]);
				}
			}
			meshAsset->AddSubmesh(
				mesh->mMaterialIndex,
				&vertices[0], 
				&indices[0], 
				vertices.size(),
				indices.size());
		}

		static inline void LoadComplexMesh(aiNode* node, const aiScene* scene, Ref<Mesh>& meshAsset, glm::vec3& boundsMin, glm::vec3& boundsMax)
		{
			for (size_t i = 0; i < node->mNumMeshes; i++) LoadSubmesh(scene->mMeshes[node->mMeshes[i]], meshAsset, boundsMin, boundsMax);
			for (size_t i = 0; i < node->mNumChildren; i++) LoadComplexMesh(node->mChildren[i], scene, meshAsset, boundsMin, boundsMax);
		}
	}

	
	/* #################### MeshAssetLoader ############################*/

	Ref<Asset> MeshAssetLoader::CreateAsset(AssetDeclaration& decl)
	{
		return Ref<Asset>();
	}

	Ref<Asset> MeshAssetLoader::LoadAsset(AssetDeclaration& decl)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(decl.FilePath.string(), MESH_IMPORT_FLAGS);
		if (!scene)
		{
			LOG_CORE_ERROR("Model ({0}) failed to load: {1}", decl.FilePath.string(), importer.GetErrorString());
			return nullptr;
		}

		glm::vec3 boundsMin = glm::vec3(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
		glm::vec3 boundsMax = glm::vec3(MIN_FLOAT, MIN_FLOAT, MIN_FLOAT);

		aiMesh* mesh = scene->mMeshes[scene->mRootNode->mChildren[0]->mMeshes[0]];
		auto meshAsset = CreateRef<Mesh>();
		Utils::LoadComplexMesh(scene->mRootNode, scene, meshAsset, boundsMin, boundsMax);

		meshAsset->SetModelSource(decl.FilePath.string());
		meshAsset->SetBounds(Bounds3(boundsMin, boundsMax));

		for (size_t i = 0; i < scene->mNumMaterials; i++)
		{
			aiMaterial* mat = scene->mMaterials[i];
			aiString materialName = mat->GetName();
			meshAsset->AddMaterial(CreateRef<Material>(std::string(materialName.C_Str())));
		}

		return meshAsset;
	}

	void MeshAssetLoader::SaveAsset(AssetDeclaration& decl, Ref<Asset> asset)
	{
		Ref<Mesh> meshAsset = std::static_pointer_cast<Mesh>(asset);
		auto& filepath = std::filesystem::path(decl.FilePath);
		auto& filename = filepath.filename().string();
		auto& fileDir = filepath.parent_path().string();

		nlohmann::json j;
		j["id"] = (uint64_t)decl.ID;
		j["filepath"] = decl.FilePath;
		j["type"] = decl.Type;

		auto& materials = meshAsset->GetMaterials();
		if (materials.size() > 0)
		{
			for (size_t i = 0; i < materials.size(); i++) 
			{
				j["material_" + i] = (uint64_t)materials[i]->m_ID;
			}
		}

		auto& outputFilename = fileDir + "\\" + decl.Name + ".mgasset";
		std::ofstream o(outputFilename);
		o << std::setw(4) << j << std::endl;
	}

	/* #################################################################*/

	/* #################### TextureAssetLoader #########################*/

	Ref<Asset> TextureAssetLoader::CreateAsset(AssetDeclaration& decl)
	{
		return Ref<Asset>();
	}

	Ref<Asset> TextureAssetLoader::LoadAsset(AssetDeclaration& decl)
	{
		int width, height, bitDepth;
		uint8_t* textureData = nullptr;
		textureData = stbi_load((decl.FilePath.string()).c_str(), &width, &height, &bitDepth, 0);

		if (!textureData) {
			LOG_CORE_ERROR("AssetManager.cpp | Failed to find: {0}", decl.FilePath.string());
			return nullptr;
		}
		else {
			TextureSpecs specs;
			specs.Width = width;
			specs.Height = height;
			specs.BitDepth = bitDepth;
			specs.FileLocation = decl.FilePath.string();
			specs.TextureFormat = TextureFormat::RGB;

			Ref<Texture2D> texture = Texture2D::Create(specs);
			texture->loadData(textureData, width, height, bitDepth);

			stbi_image_free(textureData);

			texture->m_ID = decl.ID;
			return texture;
		}
	}

	void TextureAssetLoader::SaveAsset(AssetDeclaration& decl, Ref<Asset> asset)
	{
		Ref<Texture2D> textureAsset = std::static_pointer_cast<Texture2D>(asset);
		auto& filepath = std::filesystem::path(decl.FilePath);
		auto& filename = filepath.filename().string();
		auto& fileDir = filepath.parent_path().string();

		nlohmann::json j;
		j["id"] = (uint64_t)decl.ID;
		j["filepath"] = decl.FilePath;
		j["type"] = decl.Type;
		j["texture_type"] = Utils::TextureTypeToString(textureAsset->getType());
		j["texture_wrap"] = Utils::TextureWrapToString(textureAsset->getTextureWrap());
		j["texture_filter"] = Utils::TextureFilterToString(textureAsset->getTextureFilter());

		auto& outputFilename = fileDir + "\\" + decl.Name + ".mgasset";
		if (!FileSystem::IsFileExist(fileDir)) FileSystem::MakeDir(fileDir);

		std::ofstream o(outputFilename);
		o << std::setw(4) << j << std::endl;
	}

	/* ##################################################################*/

	/* #################### MaterialAssetLoader #########################*/

	Ref<Asset> MaterialAssetLoader::CreateAsset(AssetDeclaration& decl)
	{
		return Ref<Asset>();
	}

	Ref<Asset> MaterialAssetLoader::LoadAsset(AssetDeclaration& decl)
	{
		return Ref<Asset>();
	}

	void MaterialAssetLoader::SaveAsset(AssetDeclaration& decl, Ref<Asset> asset)
	{
		Ref<Material> textureAsset = std::static_pointer_cast<Material>(asset);
		auto& filepath = std::filesystem::path(decl.FilePath);
		auto& filename = filepath.filename().string();
		auto& fileDir = filepath.parent_path().string();

		nlohmann::json j;
		j["id"] = (uint64_t)decl.ID;
		j["filepath"] = decl.FilePath;
		j["type"] = decl.Type;

		auto& outputFilename = fileDir + "\\" + decl.Name + ".mgasset";
		if (!FileSystem::IsFileExist(fileDir)) FileSystem::MakeDir(fileDir);

		std::ofstream o(outputFilename);
		o << std::setw(4) << j << std::endl;
	}

	/* ##################################################################*/
}
