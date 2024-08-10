#pragma once

#include "Moongoose/Core.h"
#include "Moongoose/Log.h"
#include <unordered_map>

namespace Moongoose {

	enum class AssetType : uint16_t
	{
		None			= 0,
		Scene			= 1,
		Mesh			= 2,
		Material		= 3,
		Texture			= 4,
		EnvMap			= 5,
		CubeMapTexture	= 6,
	};

	static std::unordered_map<std::string, AssetType> s_AssetExtensionMap =
	{
		// Mesh Types
		{ ".fbx", AssetType::Mesh },
		{ ".obj", AssetType::Mesh },

		// Texture Image Types
		{ ".png", AssetType::Texture },
		{ ".jpg", AssetType::Texture },
		{ ".tga", AssetType::Texture },
		{ ".jpeg", AssetType::Texture },
		{ ".hdr", AssetType::EnvMap },
	};

	namespace Utils {

		inline static AssetType AssetTypeFromString(const std::string& assetType)
		{
			if (assetType == "None")			return AssetType::None;
			if (assetType == "Scene")			return AssetType::Scene;
			if (assetType == "Mesh")			return AssetType::Mesh;
			if (assetType == "Material")		return AssetType::Material;
			if (assetType == "Texture")			return AssetType::Texture;
			if (assetType == "EnvMap")			return AssetType::EnvMap;

			MG_CORE_ASSERT(false, "Unknown Asset Type")
			return AssetType::None;
		}

		inline static const char* AssetTypeToString(const AssetType assetType)
		{
			switch (assetType)
			{
			case AssetType::None:			return "None";
			case AssetType::Scene:			return "Scene";
			case AssetType::Mesh:			return "Mesh";
			case AssetType::Material:		return "Material";
			case AssetType::Texture:		return "Texture";
			case AssetType::EnvMap:			return "EnvMap";
			}

			MG_CORE_ASSERT(false, "Unknown Asset Type")
			return "None";
		}

	}

}