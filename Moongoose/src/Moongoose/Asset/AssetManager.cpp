#include "mgpch.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "AssetManager.h"
#include "Moongoose/Log.h"

namespace Moongoose {

	AssetManager::AssetManager() {
		s_AssetLoaders[AssetType::Mesh] = CreateScope<MeshAssetLoader>();
		s_AssetLoaders[AssetType::Texture] = CreateScope<TextureAssetLoader>();
	}
	
	Ref<Shader> AssetManager::LoadShader(std::string vertexShaderSource, std::string fragmentShaderSource, PolygonMode polygonMode, ShaderType shaderType)
	{
		return CreateRef<Shader>(ShaderSpecs{ shaderType, vertexShaderSource, fragmentShaderSource, polygonMode });
	}	

	std::unordered_map<UUID, AssetDeclaration> AssetManager::s_AssetRegistry;
	std::unordered_map<UUID, Ref<Asset>> AssetManager::s_LoadedAssets;
	std::unordered_map<UUID, Ref<Asset>> AssetManager::s_MemoryAssets;
	std::unordered_map<AssetType, Scope<AssetLoader>> AssetManager::s_AssetLoaders;
}
