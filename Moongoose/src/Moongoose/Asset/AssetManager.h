#pragma once

#include "Moongoose/Core.h"
#include "Asset.h"
#include "AssetLoader.h"

#include "Moongoose/Renderer/Mesh.h"
#include "Moongoose/Renderer/Texture.h"
#include "Moongoose/Renderer/Shader.h"

namespace Moongoose {

	class AssetManager
	{
	public:
		static AssetManager& Get()
		{
			static AssetManager manager;
			return manager;
		}

		static AssetDeclaration ReadDeclarationFromFile(const std::filesystem::path& declFile);

		void BuildAssetRegistry();
		void LoadAsset(AssetDeclaration& decl);
		void LoadAssetFromFile(AssetDeclaration& decl);
		void ReloadAsset(AssetDeclaration& decl);
		void SaveAsset(const Ref<Asset>& asset);
		const std::unordered_map<UUID, Ref<Asset>>& GetLoadedAssets();

		template<typename T>
		Ref<T> GetDefaultAsset()
		{
			return CAST_REF(T, s_AssetLoaders[T::GetStaticAssetType()]->GetDefaultAsset());
		}


		template<typename T>
		AssetDeclaration CreateAssetDeclaration(const std::string& assetName, const std::string& path)
		{
			static_assert(std::is_base_of_v<Asset, T>, "Type must derive from Asset!");

			AssetDeclaration decl;
			decl.ID = UUID();
			decl.Name = assetName;
			decl.FilePath = path;
			decl.Type = T::GetStaticAssetType();
			decl.IsDataLoaded = false;
			decl.IsMemoryAsset = false;

			return decl;
		}

		template<typename T>
		Ref<T> CreateAsset(const std::string& assetName, const std::string& filePath)
		{
			static_assert(std::is_base_of_v<Asset, T>, "Type must derive from Asset!");

			AssetDeclaration decl = CreateAssetDeclaration<T>(assetName, filePath);
			Ref<T> asset = CAST_REF(T, s_AssetLoaders[T::GetStaticAssetType()]->CreateAsset(decl));	

			decl.IsDataLoaded = true;
			s_AssetRegistry[decl.ID] = decl;
			s_LoadedAssets[decl.ID] = asset;

			return asset;
		}

		template<typename T>
		Ref<T> LoadAssetById(const UUID& id)
		{
			return LoadAsset<T>(s_AssetRegistry[id]);
		}

		template<typename T>
		Ref<T> LoadAssetFromFileById(const UUID& id)
		{
			return LoadAssetFromFile<T>(s_AssetRegistry[id]);
		}

		template<typename T>
		Ref<T> LoadAssetFromFile(AssetDeclaration& decl)
		{
			Ref<T> asset = CAST_REF(T, s_AssetLoaders[T::GetStaticAssetType()]->LoadAssetFromFile(decl));
			asset->m_ID = decl.ID;
			asset->m_Name = decl.Name;
			decl.IsDataLoaded = true;

			s_LoadedAssets[decl.ID] = asset;

			return asset;
		}

		template<typename T>
		Ref<T> LoadAsset(AssetDeclaration& decl)
		{
			static_assert(std::is_base_of_v<Asset, T>, "Type must derive from Asset!");
			Ref<T> asset = CAST_REF(T, s_AssetLoaders[T::GetStaticAssetType()]->LoadAsset(decl));
			asset->m_ID = decl.ID;
			asset->m_Name = decl.Name;

			decl.IsDataLoaded = true;
			s_AssetRegistry[decl.ID] = decl;
			s_LoadedAssets[decl.ID] = asset;

			return asset;
		}

		void RenameAsset(const UUID& assetId, const std::string& newName)
		{
			AssetDeclaration& decl = s_AssetRegistry[assetId];
			decl.Name = newName;
			decl.DeclFilePath = decl.DeclFilePath.parent_path() / (newName + ".mgasset");

			if (decl.IsDataLoaded) SaveAsset(s_LoadedAssets[assetId]);
		}

		template<typename T>
		void ReloadAsset(const UUID assetId)
		{
			static_assert(std::is_base_of_v<Asset, T>, "Type must derive from Asset!");
			ReloadAsset(s_LoadedAssets[assetId]);
		}

		template<typename T>
		void SaveAsset(Ref<T> asset)
		{
			static_assert(std::is_base_of<Asset, T>::value, "Type must derive from Asset!");
			s_AssetLoaders[T::GetStaticAssetType()]->SaveAsset(asset);
		}

		template<typename T>
		std::vector<std::pair<std::string, AssetDeclaration>> GetAssetDeclByType()
		{
			std::vector < std::pair<std::string, AssetDeclaration>> list;
			for (auto& item : s_AssetsByType[T::GetStaticAssetType()])
			{
				list.push_back({ item.Name, item });
			}
			return list;
		}

		const std::unordered_map<UUID, AssetDeclaration>& GetAssetRegistry()
		{
			return s_AssetRegistry;
		}

		const std::vector<AssetDeclaration>& GetAssetsByFolder(const std::filesystem::path& folder)
		{
			return s_AssetsByFolder[folder];
		}

		AssetDeclaration& GetDeclById(const UUID& assetId)
		{
			return s_AssetRegistry[assetId];
		}

		Ref<Asset> GetAssetById(const UUID& assetId)
		{
			AssetDeclaration decl = GetDeclById(assetId);
			if (!decl.IsDataLoaded) LoadAssetFromFile(decl);
			return s_LoadedAssets[assetId];
		}

		template<typename T>
		Ref<T> GetAssetById(const UUID& assetId)
		{
			return std::static_pointer_cast<T>(GetAssetById(assetId));
		}

		void SetSelectedAsset(const Ref<Asset>& selectedAsset)
		{
			m_SelectedAsset = selectedAsset;
		}

		const Ref<Asset>& GetSelectedAsset() const
		{
			return m_SelectedAsset;
		}


	private:
		AssetManager();
		~AssetManager() = default;
	
	private:
		std::unordered_map<UUID, AssetDeclaration> s_AssetRegistry;
		std::unordered_map<AssetType, std::vector<AssetDeclaration>> s_AssetsByType;
		std::unordered_map<std::filesystem::path, std::vector<AssetDeclaration>> s_AssetsByFolder;

		std::unordered_map<UUID, Ref<Asset>> s_LoadedAssets;
		std::unordered_map<UUID, Ref<Asset>> s_MemoryAssets;
		std::unordered_map<AssetType, Scope<AssetLoader>> s_AssetLoaders;

		Ref<Asset> m_SelectedAsset;
	};
}