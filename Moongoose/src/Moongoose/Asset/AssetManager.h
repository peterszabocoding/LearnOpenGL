#pragma once

#include "Moongoose/Core.h"
#include "Asset.h"
#include "AssetLoader.h"

#include "Moongoose/Renderer/Mesh.h"
#include "Moongoose/Renderer/Texture.h"
#include "Moongoose/Renderer/Shader.h"
#include "Moongoose/Util/FileSystem.h"

namespace Moongoose {

	class AssetManager
	{
	public:
		static AssetManager& Get()
		{
			static AssetManager manager;
			return manager;
		}

		template<typename T>
		Ref<T> LoadAsset(const std::string& assetName, const std::string& filePath)
		{
			static_assert(std::is_base_of<Asset, T>::value, "Type must derive from Asset!");
			AssetDeclaration decl;
			decl.ID = UUID();
			decl.Name = assetName;
			decl.FilePath = filePath;
			decl.IsDataLoaded = true;
			decl.Type = T::GetStaticAssetType();

			Ref<T> asset = std::static_pointer_cast<T>(s_AssetLoaders[T::GetStaticAssetType()]->LoadAsset(decl));
			asset->m_ID = decl.ID;
			asset->m_Name = decl.Name;

			s_LoadedAssets[asset->m_ID] = asset;
			s_AssetRegistry[decl.ID] = decl;

			s_AssetLoaders[T::GetStaticAssetType()]->SaveAsset(decl, asset);

			return asset;
		}

		template<typename T>
		void SaveAsset(Ref<T> asset)
		{
			static_assert(std::is_base_of<Asset, T>::value, "Type must derive from Asset!");
			auto decl = GetDeclByID(asset->m_ID);
			s_AssetLoaders[T::GetStaticAssetType()]->SaveAsset(decl, asset);
		}

		const std::unordered_map<UUID, Ref<Asset>>& GetLoadedAssets() const { return s_LoadedAssets; }
		const AssetDeclaration& GetDeclByID(const UUID& assetID) { return s_AssetRegistry[assetID]; }

		void SetSelectedAsset(Ref<Asset> selectedAsset) { m_SelectedAsset = selectedAsset; }
		const Ref<Asset>& GetSelectedAsset() const { return m_SelectedAsset; }

	private:
		AssetManager();
		~AssetManager() = default;
	
	private:
		static std::unordered_map<UUID, AssetDeclaration> s_AssetRegistry;
		static std::unordered_map<UUID, Ref<Asset>> s_LoadedAssets;
		static std::unordered_map<UUID, Ref<Asset>> s_MemoryAssets;
		static std::unordered_map<AssetType, Scope<AssetLoader>> s_AssetLoaders;

		Ref<Asset> m_SelectedAsset;
	};
}