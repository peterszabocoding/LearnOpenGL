#include "mgpch.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "AssetManager.h"
#include "Moongoose/Log.h"

#include <nlohmann/json.hpp>

#include "Moongoose/Util/FileSystem.h"

namespace Moongoose {

	AssetManager::AssetManager() {
		s_AssetLoaders[AssetType::Mesh] = CreateScope<MeshAssetLoader>();
		s_AssetLoaders[AssetType::Texture] = CreateScope<TextureAssetLoader>();
		s_AssetLoaders[AssetType::Material] = CreateScope<MaterialAssetLoader>();
	}

	void AssetManager::BuildAssetRegistry()
	{
		const auto files = FileSystem::GetFilesFromDirectoryDeep("Content\\", ".mgasset");
		for (auto& file : files)
		{
			AssetDeclaration decl = ReadDeclarationFromFile(file);
			s_AssetRegistry[decl.ID] = decl;
		}
	}

	void AssetManager::LoadAsset(AssetDeclaration& decl)
	{
		switch (decl.Type) {
			case AssetType::Mesh:
				LoadAsset<Mesh>(decl);
				break;
			case AssetType::Material:
				LoadAsset<Material>(decl);
				break;
			case AssetType::Texture:
				LoadAsset<Texture2D>(decl);
				break;
			default:;
		}
	}

	AssetDeclaration AssetManager::ReadDeclarationFromFile(const std::filesystem::path& declFile)
	{
		std::string declJsonString = FileSystem::ReadFile(declFile);
		auto json = nlohmann::json::parse(declJsonString);

		AssetDeclaration decl;
		std::string id = json["id"];
		decl.ID = UUID(id);
		decl.Name = json["name"];
		decl.Type = Utils::AssetTypeFromString(json["type"]);
		decl.DeclFilePath = declFile.string();
		decl.FilePath = std::filesystem::path(std::string(json["source"]));

		return decl;
	}
}
