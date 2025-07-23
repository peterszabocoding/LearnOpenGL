#include "pch.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "AssetManager.h"
#include "LearnOpenGL/Log.h"

#include <nlohmann/json.hpp>
#include "LearnOpenGL/Util/FileSystem.h"

namespace LearnOpenGL {

	void AssetManager::RenameAsset(const UUID& assetId, const std::string& newName)
	{
		AssetDeclaration& decl = s_AssetRegistry[assetId];
		RenameAsset(decl, newName);
	}

	void AssetManager::RenameAsset(AssetDeclaration& decl, const std::string& newName)
	{
		decl.name = newName;

		std::filesystem::path newFilePath = decl.declFilePath.parent_path() / (newName + ".mgasset");
		FileSystem::RenameSingleFile(decl.declFilePath, newFilePath.string());
		decl.declFilePath = newFilePath;

		if (decl.isDataLoaded) SaveAsset(s_LoadedAssets[decl.id]);
	}

	const std::unordered_map<UUID, AssetDeclaration>& AssetManager::GetAssetRegistry()
	{
		return s_AssetRegistry;
	}

	const std::vector<AssetDeclaration>& AssetManager::GetAssetsByFolder(const std::filesystem::path& folder)
	{
		return s_AssetsByFolder[folder];
	}

	AssetDeclaration& AssetManager::GetDeclById(const UUID& assetId)
	{
		return s_AssetRegistry[assetId];
	}

	Ref<Asset> AssetManager::GetAssetById(const UUID& assetId)
	{
		AssetDeclaration decl = GetDeclById(assetId);
		if (!decl.isDataLoaded) LoadAssetFromFile(decl);
		return s_LoadedAssets[assetId];
	}

	void AssetManager::SetSelectedAsset(const Ref<Asset>& selectedAsset)
	{
		m_SelectedAsset = selectedAsset;
	}

	const Ref<Asset>& AssetManager::GetSelectedAsset() const
	{
		return m_SelectedAsset;
	}

	AssetManager::AssetManager() {
		s_AssetLoaders[AssetType::Mesh] = CreateScope<MeshAssetLoader>();
		s_AssetLoaders[AssetType::Texture] = CreateScope<TextureAssetLoader>();
		s_AssetLoaders[AssetType::Material] = CreateScope<MaterialAssetLoader>();
	}

	void AssetManager::BuildAssetRegistry()
	{
		s_AssetRegistry.clear();
		s_AssetsByFolder.clear();
		s_AssetsByType.clear();

		const auto files = FileSystem::GetFilesFromDirectoryDeep("Content\\", ".mgasset");
		for (auto& file : files)
		{
			AssetDeclaration decl = ReadDeclarationFromFile(file);
			s_AssetRegistry[decl.id] = decl;

			s_AssetsByType[decl.type].push_back(decl);

			std::filesystem::path fileDir = file.parent_path();
			if (!is_directory(fileDir)) continue;

			const bool isListAvailable = s_AssetsByFolder.find(fileDir) != s_AssetsByFolder.end();
			if (!isListAvailable) s_AssetsByFolder[fileDir] = std::vector<AssetDeclaration>();

			s_AssetsByFolder[fileDir].push_back(decl);
		}
	}

	void AssetManager::ReloadAsset(AssetDeclaration& decl)
	{
		s_AssetLoaders[decl.type]->ReloadAsset(decl);
	}

	void AssetManager::DeleteAsset(AssetDeclaration& decl)
	{
	}

	void AssetManager::SaveAsset(const Ref<Asset>& asset)
	{
		s_AssetLoaders[asset->GetAssetType()]->SaveAsset(asset);
	}

	void AssetManager::LoadAsset(AssetDeclaration& decl)
	{
		switch (decl.type) {
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

	void AssetManager::LoadAssetFromFile(AssetDeclaration& decl)
	{
		switch (decl.type) {
		case AssetType::Mesh:
			LoadAssetFromFile<Mesh>(decl);
			break;
		case AssetType::Material:
			LoadAssetFromFile<Material>(decl);
			break;
		case AssetType::Texture:
			LoadAssetFromFile<Texture2D>(decl);
			break;
		default:;
		}
	}

	const std::unordered_map<UUID, Ref<Asset>>& AssetManager::GetLoadedAssets()
	{
		return s_LoadedAssets;
	}

	AssetDeclaration AssetManager::ReadDeclarationFromFile(const std::filesystem::path& declFile)
	{
		std::string declJsonString = FileSystem::ReadFile(declFile);
		auto json = nlohmann::json::parse(declJsonString);

		AssetDeclaration decl;
		std::string id = json["id"];
		decl.id = UUID(id);
		decl.name = json["name"];
		decl.type = Utils::AssetTypeFromString(json["type"]);
		decl.declFilePath = declFile.string();
		decl.filePath = std::filesystem::path(std::string(json["source"]));

		return decl;
	}
}
