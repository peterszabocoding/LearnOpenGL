#pragma once
#include "UUID.h"
#include "AssetTypes.h"
#include <filesystem>

typedef std::filesystem::path Path;

namespace Moongoose {

	struct AssetDeclaration
	{
		UUID id = 0;
		std::string name;
		AssetType type = AssetType::None;

		Path declFilePath;
		Path filePath;

		bool isDataLoaded = false;
		bool isMemoryAsset = false;
		bool IsValid() const { return id != 0 && !isMemoryAsset; }
	};

	class Asset
	{
	public:
		virtual ~Asset() = default;
		virtual AssetType GetAssetType() const { return AssetType::None; }
		static AssetType GetStaticAssetType() { return AssetType::None; }

		virtual bool operator==(const Asset& other) const { return m_Id == other.m_Id; }
		virtual bool operator!=(const Asset& other) const { return !(*this == other); }

		UUID m_Id;
		std::string m_Name;
	};
}