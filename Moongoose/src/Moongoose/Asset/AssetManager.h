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

		template<typename T>
		Ref<T> CreateAsset(const std::string& assetName, const std::string& filePath)
		{
			static_assert(std::is_base_of_v<Asset, T>, "Type must derive from Asset!");

			AssetDeclaration decl;
			decl.ID = UUID();
			decl.Name = assetName;
			decl.FilePath = filePath;
			decl.Type = T::GetStaticAssetType();

			return LoadAsset<T>(decl);
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
			Ref<T> asset = std::static_pointer_cast<T>(s_AssetLoaders[T::GetStaticAssetType()]->LoadAssetFromFile(decl));
			asset->m_ID = decl.ID;
			asset->m_Name = decl.Name;
			decl.IsDataLoaded = true;

			s_LoadedAssets[decl.ID] = asset;
			s_AssetLoaders[T::GetStaticAssetType()]->SaveAsset(asset);

			return asset;
		}

		template<typename T>
		Ref<T> LoadAsset(AssetDeclaration& decl)
		{
			static_assert(std::is_base_of_v<Asset, T>, "Type must derive from Asset!");
			Ref<T> asset = std::static_pointer_cast<T>(s_AssetLoaders[T::GetStaticAssetType()]->LoadAsset(decl));
			asset->m_ID = decl.ID;
			asset->m_Name = decl.Name;

			decl.IsDataLoaded = true;
			s_AssetRegistry[decl.ID] = decl;
			s_LoadedAssets[decl.ID] = asset;

			s_AssetLoaders[T::GetStaticAssetType()]->SaveAsset(asset);

			return asset;
		}

		void ReloadAsset(AssetDeclaration& decl)
		{
			s_AssetLoaders[decl.Type]->ReloadAsset(decl);
		}

		template<typename T>
		void ReloadAsset(UUID assetId)
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

		const std::unordered_map<UUID, Ref<Asset>>& GetLoadedAssets()
		{
			return s_LoadedAssets;
		}

		const std::unordered_map<UUID, AssetDeclaration>& GetAssetRegistry()
		{
			return s_AssetRegistry;
		}

		AssetDeclaration& GetDeclByID(const UUID& assetId)
		{
			return s_AssetRegistry[assetId];
		}

		Ref<Asset> GetAssetById(const UUID& assetId)
		{
			AssetDeclaration decl = GetDeclByID(assetId);
			if (!decl.IsDataLoaded)
			{
				LoadAsset(decl);
			}
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
		std::unordered_map<UUID, Ref<Asset>> s_LoadedAssets;
		std::unordered_map<UUID, Ref<Asset>> s_MemoryAssets;
		std::unordered_map<AssetType, Scope<AssetLoader>> s_AssetLoaders;

		Ref<Asset> m_SelectedAsset;
	};
}