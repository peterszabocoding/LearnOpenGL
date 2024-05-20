#pragma once
#include "UUID.h"
#include "AssetTypes.h"
#include <filesystem>

namespace Moongoose {

	class Asset
	{
	public:
		virtual AssetType getAssetType() const { return AssetType::None; }
		static AssetType getStaticAssetType() { return AssetType::None; }

		virtual bool operator==(const Asset& other) const { return m_ID == other.m_ID; }
		virtual bool operator!=(const Asset& other) const { return !(*this == other); }

	public:
		UUID m_ID;
		std::string m_Name = "";
	};

	struct AssetDeclaration
	{
		UUID ID = 0;
		std::string Name = "";
		AssetType Type = AssetType::None;

		std::filesystem::path FilePath;
		bool IsDataLoaded = false;
		bool IsMemoryAsset = false;

		bool IsValid() const { return ID != 0 && !IsMemoryAsset; }
	};

}