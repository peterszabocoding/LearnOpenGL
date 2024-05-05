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

		Ref<Mesh> LoadMesh(std::string meshPath);
		Ref<Texture2D> LoadTexture2D(const std::string& filepath, TextureFormat textureFormat);
		Ref<Shader> LoadShader(std::string vertexShaderSource, std::string fragmentShaderSource, PolygonMode polygonMode = PolygonMode::FILL, ShaderType shaderType = ShaderType::STATIC);
	
		template<typename T>
		static Ref<T> LoadAsset(const std::string& filePath)
		{
			static_assert(std::is_base_of<Asset, T>::value, "Type must derive from Asset!");
			AssetDeclaration decl;
			decl.ID = UUID();
			decl.FilePath = filePath;
			decl.IsDataLoaded = true;
			decl.Type = T::GetStaticAssetType();

			s_AssetRegistry[decl.ID] = decl;

			Ref<T> asset = std::static_pointer_cast<T>(s_AssetLoaders[T::GetStaticAssetType()]->LoadAsset(filePath));
			asset->m_ID = decl.ID;
			s_LoadedAssets[asset->m_ID] = asset;

			return asset;
		}

	private:
		AssetManager();
		~AssetManager() = default;
	
	private:

		static std::unordered_map<UUID, AssetDeclaration> s_AssetRegistry;
		static std::unordered_map<UUID, Ref<Asset>> s_LoadedAssets;
		static std::unordered_map<UUID, Ref<Asset>> s_MemoryAssets;
		static std::unordered_map<AssetType, Scope<AssetLoader>> s_AssetLoaders;
	
	};

}