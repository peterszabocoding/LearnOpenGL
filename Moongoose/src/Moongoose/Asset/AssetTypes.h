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
		StaticMesh		= 3,
		MeshSource		= 4,
		Material		= 5,
		Texture			= 6,
		EnvMap			= 7,
	};

	static std::unordered_map<std::string, AssetType> s_AssetExtensionMap =
	{
		// Mesh Tyes
		{ ".fbx", AssetType::MeshSource },
		{ ".obj", AssetType::MeshSource },

		// Texture Image Types
		{ ".png", AssetType::Texture },
		{ ".jpg", AssetType::Texture },
		{ ".jpeg", AssetType::Texture },
		{ ".hdr", AssetType::EnvMap },
	};

	namespace Utils {

		inline static AssetType AssetTypeFromString(const std::string& assetType)
		{
			if (assetType == "None")			return AssetType::None;
			if (assetType == "Scene")			return AssetType::Scene;
			if (assetType == "Mesh")			return AssetType::Mesh;
			if (assetType == "StaticMesh")		return AssetType::StaticMesh;
			if (assetType == "MeshSource")		return AssetType::MeshSource;
			if (assetType == "Material")		return AssetType::Material;
			if (assetType == "Texture")			return AssetType::Texture;
			if (assetType == "EnvMap")			return AssetType::EnvMap;

			MG_CORE_ASSERT(false, "Unknown Asset Type");
			return AssetType::None;
		}

		inline static const char* AssetTypeToString(AssetType assetType)
		{
			switch (assetType)
			{
			case AssetType::None:			return "None";
			case AssetType::Scene:			return "Scene";
			case AssetType::Mesh:			return "Mesh";
			case AssetType::StaticMesh:		return "StaticMesh";
			case AssetType::MeshSource:		return "MeshSource";
			case AssetType::Material:		return "Material";
			case AssetType::Texture:		return "Texture";
			case AssetType::EnvMap:			return "EnvMap";
			}

			MG_CORE_ASSERT(false, "Unknown Asset Type");
			return "None";
		}

	}

}