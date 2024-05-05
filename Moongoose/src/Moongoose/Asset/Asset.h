#pragma once
#include "UUID.h"
#include "AssetTypes.h"

namespace Moongoose {

	class Asset
	{
	public:
		
		virtual ~Asset() {};

		virtual AssetType getAssetType() const { return AssetType::None; }
		static AssetType getStaticAssetType() { return AssetType::None; }

		virtual bool operator==(const Asset& other) const { return m_ID == other.m_ID; }
		virtual bool operator!=(const Asset& other) const { return !(*this == other); }

	public:
		UUID m_ID;
	};

	struct AssetDeclaration
	{
		UUID ID = 0;
		AssetType Type;

		std::filesystem::path FilePath;
		bool IsDataLoaded = false;
		bool IsMemoryAsset = false;

		bool IsValid() const { return ID != 0 && !IsMemoryAsset; }
	};

}