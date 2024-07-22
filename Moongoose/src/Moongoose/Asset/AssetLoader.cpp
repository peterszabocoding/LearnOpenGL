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
#include "Moongoose/Asset/AssetManager.h"

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

		static void LoadSubmesh(const aiScene* scene, const aiMesh* mesh, const Ref<Mesh>& meshAsset, glm::vec3& boundsMin, glm::vec3& boundsMax)
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

		static void LoadComplexMesh(aiNode* node, const aiScene* scene, Ref<Mesh>& meshAsset, glm::vec3& boundsMin, glm::vec3& boundsMax)
		{
			for (size_t i = 0; i < node->mNumMeshes; i++) LoadSubmesh(scene, scene->mMeshes[node->mMeshes[i]], meshAsset, boundsMin, boundsMax);
			for (size_t i = 0; i < node->mNumChildren; i++) LoadComplexMesh(node->mChildren[i], scene, meshAsset, boundsMin, boundsMax);
		}

		static void LoadMeshMaterials(const aiScene* scene, const Ref<Mesh>& meshAsset)
		{
			for (size_t i = 0; i < scene->mNumMaterials; i++)
			{
				meshAsset->AddMaterial(nullptr);
			}
		}

		static uint8_t* LoadImageTexture(const std::string& filePath, int& width, int& height, int& bitDepth)
		{
			return stbi_load(filePath.c_str(), &width, &height, &bitDepth, 0);
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

		Ref<Mesh> meshAsset = CreateRef<Mesh>();
		Utils::LoadComplexMesh(scene->mRootNode, scene, meshAsset, boundsMin, boundsMax);

		meshAsset->SetModelSource(decl.FilePath.string());
		meshAsset->SetBounds(Bounds3(boundsMin, boundsMax));

		Utils::LoadMeshMaterials(scene, meshAsset);

		return meshAsset;
	}

	Ref<Asset> MeshAssetLoader::LoadAssetFromFile(AssetDeclaration& decl)
	{
		std::string assetJsonString = FileSystem::ReadFile(decl.DeclFilePath);
		auto assetJson = nlohmann::json::parse(assetJsonString);
		std::vector <Ref<Material>> materials;
		std::vector<MaterialSlot> materialSlots;
		auto materialListJson = assetJson["materials"];

		if (!materialListJson.empty())
		{
			for (auto materialJson : materialListJson)
			{
				UUID materialId = materialJson[0].get<std::string>();
				std::string materialSlotName = materialJson[1].get<std::string>();

				auto& materialDecl = AssetManager::Get().GetDeclByID(materialId);

				if (materialDecl.Type == AssetType::None) continue;
				if (!materialDecl.IsDataLoaded) AssetManager::Get().LoadAssetById<Material>(materialId);

				auto mat = AssetManager::Get().GetAssetById<Material>(materialId);

				materials.push_back(mat);
				materialSlots.push_back({ materialSlotName, mat });
			}
				
		}

		Ref<Mesh> meshAsset = std::static_pointer_cast<Mesh>(LoadAsset(decl));
		for (size_t i = 0; i < materialSlots.size(); i++)
		{
			meshAsset->AddMaterial(i, materialSlots[i].material, materialSlots[i].name);
		}
		return meshAsset;
	}

	Ref<Asset> MeshAssetLoader::ReloadAsset(AssetDeclaration& decl)
	{
		return AssetManager::Get().GetAssetById(decl.ID);
	}

	Ref<Asset> MeshAssetLoader::GetDefaultAsset()
	{
		return AssetManager::Get().GetAssetById<Mesh>(UUID(17706748999392867240));
	}

	void MeshAssetLoader::SaveAsset(const Ref<Asset> asset)
	{
		AssetDeclaration& decl = AssetManager::Get().GetDeclByID(asset->m_ID);
		const Ref<Mesh> meshAsset = std::static_pointer_cast<Mesh>(asset);

		const auto& filepath = std::filesystem::path(decl.FilePath);
		auto filename = filepath.filename().string();
		const auto& fileDir = filepath.parent_path().string();

		nlohmann::json j;
		j["id"] = std::to_string(decl.ID);
		j["name"] = decl.Name;
		j["source"] = decl.FilePath;
		j["type"] = Utils::AssetTypeToString(decl.Type);

		if (auto& materials = meshAsset->GetMaterials(); !materials.empty())
		{
			std::vector<std::tuple<std::string, std::string>> materialNamesAndIds;
			materialNamesAndIds.reserve(materials.size());

			for (auto& mat : materials)
			{
				if (!mat.material) continue;
				materialNamesAndIds.emplace_back(std::to_string(mat.material->m_ID), mat.name);
			}
			j["materials"] = materialNamesAndIds;
		}

		std::string outputFilename = fileDir + "\\" + decl.Name + ".mgasset";
		std::ofstream o(outputFilename);
		o << std::setw(4) << j << '\n';

		decl.DeclFilePath = outputFilename;
	}

	/* #################################################################*/

	/* #################### TextureAssetLoader #########################*/

	Ref<Asset> TextureAssetLoader::CreateAsset(AssetDeclaration& decl)
	{
		return LoadAsset(decl);
	}

	Ref<Asset> TextureAssetLoader::LoadAsset(AssetDeclaration& decl)
	{
		int width, height, bitDepth;
		uint8_t* textureData = Utils::LoadImageTexture(decl.FilePath.string(), width, height, bitDepth);

		if (!textureData) {
			LOG_CORE_ERROR("AssetManager.cpp | Failed to find: {0}", decl.FilePath.string());
			return nullptr;
		}

		TextureSpecs specs;
		specs.Width = width;
		specs.Height = height;
		specs.BitDepth = bitDepth;
		specs.FileLocation = decl.FilePath.string();
		specs.TextureFormat = TextureFormat::RGB;

		Ref<Texture2D> texture = Texture2D::Create();
		texture->m_ID = decl.ID;
		texture->LoadData(specs, textureData);

		stbi_image_free(textureData);
		return texture;
	}

	Ref<Asset> TextureAssetLoader::LoadAssetFromFile(AssetDeclaration& decl)
	{
		std::string assetJsonString = FileSystem::ReadFile(decl.DeclFilePath);
		auto assetJson = nlohmann::json::parse(assetJsonString);

		Ref<Texture2D> textureAsset = std::static_pointer_cast<Texture2D>(LoadAsset(decl));
		textureAsset->SetTextureFilter(Utils::TextureFilterFromString(assetJson["texture_filter"]));
		textureAsset->SetTextureWrap(Utils::TextureWrapFromString(assetJson["texture_wrap"]));

		return textureAsset;
	}

	Ref<Asset> TextureAssetLoader::ReloadAsset(AssetDeclaration& decl)
	{
		Ref<Texture2D> texture = std::static_pointer_cast<Texture2D>(AssetManager::Get().GetAssetById(decl.ID));
		texture->UnloadData();

		int width, height, bitDepth;
		uint8_t* textureData = Utils::LoadImageTexture(decl.FilePath.string(), width, height, bitDepth);

		if (!textureData) {
			LOG_CORE_ERROR("AssetManager.cpp | Failed to find: {0}", decl.FilePath.string());
			return nullptr;
		}

		TextureSpecs specs;
		specs.Width = width;
		specs.Height = height;
		specs.BitDepth = bitDepth;
		specs.FileLocation = decl.FilePath.string();
		specs.TextureFormat = TextureFormat::RGB;

		texture->LoadData(specs, textureData);

		stbi_image_free(textureData);
		return texture;
	}

	Ref<Asset> TextureAssetLoader::GetDefaultAsset()
	{
		return AssetManager::Get().GetAssetById<Texture2D>(UUID(8930786947602648694));
	}

	void TextureAssetLoader::SaveAsset(const Ref<Asset> asset)
	{
		AssetDeclaration& decl = AssetManager::Get().GetDeclByID(asset->m_ID);
		const Ref<Texture2D> textureAsset = std::static_pointer_cast<Texture2D>(asset);

		const auto& filepath = std::filesystem::path(decl.FilePath);
		auto filename = filepath.filename().string();
		const auto& fileDir = filepath.parent_path().string();

		nlohmann::json j;
		j["id"] = std::to_string(decl.ID);
		j["name"] = decl.Name;
		j["source"] = decl.FilePath;
		j["type"] = Utils::AssetTypeToString(decl.Type);
		j["texture_type"] = Utils::TextureTypeToString(textureAsset->getType());
		j["texture_wrap"] = Utils::TextureWrapToString(textureAsset->GetTextureWrap());
		j["texture_filter"] = Utils::TextureFilterToString(textureAsset->GetTextureFilter());

		const auto& outputFilename = fileDir + "\\" + decl.Name + ".mgasset";
		if (!FileSystem::IsFileExist(fileDir)) FileSystem::MakeDirectory(fileDir);

		std::ofstream o(outputFilename);
		o << std::setw(4) << j << '\n';

		decl.DeclFilePath = outputFilename;
	}

	/* ##################################################################*/

	/* #################### MaterialAssetLoader #########################*/

	Ref<Asset> MaterialAssetLoader::CreateAsset(AssetDeclaration& decl)
	{
		Ref<Material> newMaterial = CreateRef<Material>(decl.Name);
		newMaterial->m_ID = decl.ID;
		newMaterial->setAlbedo(AssetManager::Get().GetDefaultAsset<Texture2D>());
		return newMaterial;
	}

	Ref<Asset> MaterialAssetLoader::LoadAsset(AssetDeclaration& decl)
	{
		return LoadAssetFromFile(decl);
	}

	Ref<Asset> MaterialAssetLoader::LoadAssetFromFile(AssetDeclaration& decl)
	{
		std::string assetJsonString = FileSystem::ReadFile(decl.DeclFilePath);
		auto assetJson = nlohmann::json::parse(assetJsonString);

		Ref<Material> materialAsset = std::static_pointer_cast<Material>(CreateAsset(decl));
		auto& albedoJson = assetJson["albedo"];

		if (!albedoJson.empty())
		{ 
			UUID albedoTextureId = albedoJson["ID"].get<std::string>();
			AssetDeclaration albedoTextureDecl = AssetManager::Get().GetDeclByID(albedoTextureId);
			Ref<Texture2D> albedoTexture = !albedoTextureDecl.IsDataLoaded
				? AssetManager::Get().LoadAssetById<Texture2D>(albedoTextureId)
				: AssetManager::Get().GetAssetById<Texture2D>(albedoTextureId);
			
			materialAsset->setAlbedo(albedoTexture);
		}

		return materialAsset;
	}

	Ref<Asset> MaterialAssetLoader::ReloadAsset(AssetDeclaration& decl)
	{
		return AssetManager::Get().GetAssetById(decl.ID);
	}

	Ref<Asset> MaterialAssetLoader::GetDefaultAsset()
	{
		return AssetManager::Get().GetAssetById<Material>(UUID(12295842333264798360));
	}

	void MaterialAssetLoader::SaveAsset(const Ref<Asset> asset)
	{
		AssetDeclaration& decl = AssetManager::Get().GetDeclByID(asset->m_ID);
		const Ref<Material> materialAsset = CAST_REF(Material, asset);

		const auto fileDir = std::filesystem::path(decl.FilePath).parent_path();
		const auto& outputFilename = fileDir.string() + "\\" + decl.Name + ".mgasset";

		if (!FileSystem::IsFileExist(fileDir))
		{
			FileSystem::MakeDirectory(fileDir);
		}

		nlohmann::json j;
		j["id"] = std::to_string(decl.ID);
		j["name"] = decl.Name;
		j["source"] = decl.FilePath;
		j["type"] = Utils::AssetTypeToString(decl.Type);

		if (materialAsset->getAlbedo())
		{
			auto& albedoTexture = materialAsset->getAlbedo();
			j["albedo"] = {
				{ "ID", std::to_string(albedoTexture->m_ID)},
				{ "Name", albedoTexture->m_Name},
			};
		}

		std::ofstream o(outputFilename);
		o << std::setw(4) << j << '\n';
	}

	/* ##################################################################*/
}
