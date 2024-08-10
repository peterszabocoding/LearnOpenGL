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
		const aiScene* scene = importer.ReadFile(decl.filePath.string(), MESH_IMPORT_FLAGS);
		if (!scene)
		{
			LOG_CORE_ERROR("Model ({0}) failed to load: {1}", decl.filePath.string(), importer.GetErrorString());
			return nullptr;
		}

		glm::vec3 boundsMin = glm::vec3(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
		glm::vec3 boundsMax = glm::vec3(MIN_FLOAT, MIN_FLOAT, MIN_FLOAT);

		Ref<Mesh> meshAsset = CreateRef<Mesh>();
		Utils::LoadComplexMesh(scene->mRootNode, scene, meshAsset, boundsMin, boundsMax);

		meshAsset->SetModelSource(decl.filePath.string());
		meshAsset->SetBounds(Bounds3(boundsMin, boundsMax));

		Utils::LoadMeshMaterials(scene, meshAsset);

		return meshAsset;
	}

	Ref<Asset> MeshAssetLoader::LoadAssetFromFile(AssetDeclaration& decl)
	{
		std::string assetJsonString = FileSystem::ReadFile(decl.declFilePath);
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

				auto& materialDecl = AssetManager::Get().GetDeclById(materialId);

				if (materialDecl.type == AssetType::None) continue;
				if (!materialDecl.isDataLoaded) AssetManager::Get().LoadAssetById<Material>(materialId);

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
		return AssetManager::Get().GetAssetById(decl.id);
	}

	Ref<Asset> MeshAssetLoader::GetDefaultAsset()
	{
		return AssetManager::Get().GetAssetById<Mesh>(UUID(17706748999392867240));
	}

	void MeshAssetLoader::SaveAsset(const Ref<Asset> asset)
	{
		AssetDeclaration& decl = AssetManager::Get().GetDeclById(asset->m_Id);
		const Ref<Mesh> meshAsset = std::static_pointer_cast<Mesh>(asset);

		const auto& filepath = std::filesystem::path(decl.filePath);
		auto filename = filepath.filename().string();
		const auto& fileDir = filepath.parent_path().string();

		nlohmann::json j;
		j["id"] = std::to_string(decl.id);
		j["name"] = decl.name;
		j["source"] = decl.filePath;
		j["type"] = Utils::AssetTypeToString(decl.type);

		if (auto& materials = meshAsset->GetMaterials(); !materials.empty())
		{
			std::vector<std::tuple<std::string, std::string>> materialNamesAndIds;
			materialNamesAndIds.reserve(materials.size());

			for (auto& mat : materials)
			{
				if (!mat.material) continue;
				materialNamesAndIds.emplace_back(std::to_string(mat.material->m_Id), mat.name);
			}
			j["materials"] = materialNamesAndIds;
		}

		std::string outputFilename = fileDir + "\\" + decl.name + ".mgasset";
		std::ofstream o(outputFilename);
		o << std::setw(4) << j << '\n';

		decl.declFilePath = outputFilename;
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
		uint8_t* textureData = Utils::LoadImageTexture(decl.filePath.string(), width, height, bitDepth);

		if (!textureData) {
			LOG_CORE_ERROR("AssetManager.cpp | Failed to find: {0}", decl.filePath.string());
			return nullptr;
		}

		TextureSpecs specs;
		specs.width = width;
		specs.height = height;
		specs.bitDepth = bitDepth;
		specs.fileLocation = decl.filePath.string();
		specs.textureFormat = TextureFormat::RGB;

		Ref<Texture2D> texture = Texture2D::Create();
		texture->m_Id = decl.id;
		texture->LoadData(specs, textureData);

		stbi_image_free(textureData);
		return texture;
	}

	Ref<Asset> TextureAssetLoader::LoadAssetFromFile(AssetDeclaration& decl)
	{
		std::string assetJsonString = FileSystem::ReadFile(decl.declFilePath);
		auto assetJson = nlohmann::json::parse(assetJsonString);

		Ref<Texture2D> textureAsset = std::static_pointer_cast<Texture2D>(LoadAsset(decl));
		textureAsset->SetTextureFilter(assetJson.contains("texture_filter") ? Utils::TextureFilterFromString(assetJson["texture_filter"]) : TextureFilter::Linear);
		textureAsset->SetTextureWrap(assetJson.contains("texture_wrap") ? Utils::TextureWrapFromString(assetJson["texture_wrap"]) : TextureWrap::Repeat);

		return textureAsset;
	}

	Ref<Asset> TextureAssetLoader::ReloadAsset(AssetDeclaration& decl)
	{
		Ref<Texture2D> texture = std::static_pointer_cast<Texture2D>(AssetManager::Get().GetAssetById(decl.id));
		texture->UnloadData();

		int width, height, bitDepth;
		uint8_t* textureData = Utils::LoadImageTexture(decl.filePath.string(), width, height, bitDepth);

		if (!textureData) {
			LOG_CORE_ERROR("AssetManager.cpp | Failed to find: {0}", decl.filePath.string());
			return nullptr;
		}

		TextureSpecs specs;
		specs.width = width;
		specs.height = height;
		specs.bitDepth = bitDepth;
		specs.fileLocation = decl.filePath.string();
		specs.textureFormat = TextureFormat::RGB;

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
		AssetDeclaration& decl = AssetManager::Get().GetDeclById(asset->m_Id);
		const Ref<Texture2D> textureAsset = std::static_pointer_cast<Texture2D>(asset);

		const auto& filepath = std::filesystem::path(decl.filePath);
		auto filename = filepath.filename().string();
		const auto& fileDir = filepath.parent_path().string();

		nlohmann::json j;
		j["id"] = std::to_string(decl.id);
		j["name"] = decl.name;
		j["source"] = decl.filePath;
		j["type"] = Utils::AssetTypeToString(decl.type);
		j["texture_type"] = Utils::TextureTypeToString(textureAsset->getType());
		j["texture_wrap"] = Utils::TextureWrapToString(textureAsset->GetTextureWrap());
		j["texture_filter"] = Utils::TextureFilterToString(textureAsset->GetTextureFilter());

		const auto& outputFilename = fileDir + "\\" + decl.name + ".mgasset";
		if (!FileSystem::IsFileExist(fileDir)) FileSystem::MakeDirectory(fileDir);

		std::ofstream o(outputFilename);
		o << std::setw(4) << j << '\n';

		decl.declFilePath = outputFilename;
	}

	/* ##################################################################*/

	/* #################### MaterialAssetLoader #########################*/

	Ref<Asset> MaterialAssetLoader::CreateAsset(AssetDeclaration& decl)
	{
		Ref<Material> newMaterial = CreateRef<Material>(decl.name);
		newMaterial->m_Id = decl.id;
		return newMaterial;
	}

	Ref<Asset> MaterialAssetLoader::LoadAsset(AssetDeclaration& decl)
	{
		return LoadAssetFromFile(decl);
	}

	Ref<Asset> MaterialAssetLoader::LoadAssetFromFile(AssetDeclaration& decl)
	{
		std::string assetJsonString = FileSystem::ReadFile(decl.declFilePath);
		auto assetJson = nlohmann::json::parse(assetJsonString);

		Ref<Material> materialAsset = std::static_pointer_cast<Material>(CreateAsset(decl));

		// Load Albedo
		if (auto& albedoJson = assetJson["albedo"]; !albedoJson.empty())
		{
			const UUID albedoTextureId = albedoJson["ID"].get<std::string>();
			const AssetDeclaration albedoTextureDecl = AssetManager::Get().GetDeclById(albedoTextureId);

			materialAsset->m_Albedo = !albedoTextureDecl.isDataLoaded
				? AssetManager::Get().LoadAssetById<Texture2D>(albedoTextureId)
				: AssetManager::Get().GetAssetById<Texture2D>(albedoTextureId);
		}

		if (assetJson.contains("albedoColor"))
		{
			materialAsset->m_AlbedoColor = glm::vec3(
				assetJson["albedoColor"]["r"],
				assetJson["albedoColor"]["g"],
				assetJson["albedoColor"]["b"]);
		}

		// Load Normal map
		if (auto& normalJson = assetJson["normal"]; !normalJson.empty())
		{
			const UUID normalTextureId = normalJson["ID"].get<std::string>();
			const AssetDeclaration normalTextureDecl = AssetManager::Get().GetDeclById(normalTextureId);

			materialAsset->m_Normal = !normalTextureDecl.isDataLoaded
				? AssetManager::Get().LoadAssetById<Texture2D>(normalTextureId)
				: AssetManager::Get().GetAssetById<Texture2D>(normalTextureId);
		}


		// Load Metallic
		if (auto& metallicJson = assetJson["metallic"]; !metallicJson.empty())
		{
			const UUID metallicTextureId = metallicJson["ID"].get<std::string>();
			const AssetDeclaration metallicTextureDecl = AssetManager::Get().GetDeclById(metallicTextureId);

			materialAsset->m_Metallic = !metallicTextureDecl.isDataLoaded
				? AssetManager::Get().LoadAssetById<Texture2D>(metallicTextureId)
				: AssetManager::Get().GetAssetById<Texture2D>(metallicTextureId);
		}

		if (assetJson.contains("metallicValue"))
		{
			materialAsset->m_MetallicValue = assetJson["metallicValue"];
		}

		// Load Roughness
		if (auto& roughnessJson = assetJson["roughness"]; !roughnessJson.empty())
		{
			const UUID roughnessTextureId = roughnessJson["ID"].get<std::string>();
			const AssetDeclaration roughnessTextureDecl = AssetManager::Get().GetDeclById(roughnessTextureId);

			materialAsset->m_Metallic = !roughnessTextureDecl.isDataLoaded
				? AssetManager::Get().LoadAssetById<Texture2D>(roughnessTextureId)
				: AssetManager::Get().GetAssetById<Texture2D>(roughnessTextureId);
		}

		if (assetJson.contains("roughnessValue"))
		{
			materialAsset->m_RoughnessValue = assetJson["roughnessValue"];
		}

		return materialAsset;
	}

	Ref<Asset> MaterialAssetLoader::ReloadAsset(AssetDeclaration& decl)
	{
		return AssetManager::Get().GetAssetById(decl.id);
	}

	Ref<Asset> MaterialAssetLoader::GetDefaultAsset()
	{
		return AssetManager::Get().GetAssetById<Material>(UUID(12295842333264798360));
	}

	void MaterialAssetLoader::SaveAsset(const Ref<Asset> asset)
	{
		AssetDeclaration& decl = AssetManager::Get().GetDeclById(asset->m_Id);
		const Ref<Material> materialAsset = CAST_REF(Material, asset);

		const auto fileDir = std::filesystem::path(decl.filePath).parent_path();
		const auto& outputFilename = fileDir.string() + "\\" + decl.name + ".mgasset";

		if (!FileSystem::IsFileExist(fileDir))
		{
			FileSystem::MakeDirectory(fileDir);
		}

		nlohmann::json j;
		j["id"] = std::to_string(decl.id);
		j["name"] = decl.name;
		j["source"] = decl.filePath;
		j["type"] = Utils::AssetTypeToString(decl.type);

		if (materialAsset->m_Albedo)
		{
			j["albedo"] = {
				{ "ID", std::to_string(materialAsset->m_Albedo->m_Id)},
				{ "Name", materialAsset->m_Albedo->m_Name}
			};
		}

		j["albedoColor"] = {
			{ "r", materialAsset->m_AlbedoColor.r },
			{ "g", materialAsset->m_AlbedoColor.g },
			{ "b", materialAsset->m_AlbedoColor.b }
		};

		if (materialAsset->m_Normal)
		{
			j["normal"] = {
				{ "ID", std::to_string(materialAsset->m_Normal->m_Id)},
				{ "Name", materialAsset->m_Normal->m_Name},
			};
		}

		if (materialAsset->m_Metallic)
		{
			j["metallic"] = {
				{ "ID", std::to_string(materialAsset->m_Metallic->m_Id)},
				{ "Name", materialAsset->m_Metallic->m_Name}
			};
		}

		j["metallicValue"] = materialAsset->m_MetallicValue;

		if (materialAsset->m_Roughness)
		{
			j["roughness"] = {
				{ "ID", std::to_string(materialAsset->m_Roughness->m_Id)},
				{ "Name", materialAsset->m_Roughness->m_Name}
			};
		}
		j["roughnessValue"] = materialAsset->m_RoughnessValue;

		std::ofstream o(outputFilename);
		o << std::setw(4) << j << '\n';

		/* ##################################################################*/
	}
}
