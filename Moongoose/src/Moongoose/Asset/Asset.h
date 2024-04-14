#pragma once
#include "UUID.h"
#include "AssetTypes.h"

namespace Moongoose {

	class Asset
	{
	public:
		
		virtual ~Asset() {};

		virtual AssetType getAssetType() const { return AssetType::None; }

		virtual bool operator==(const Asset& other) const
		{
			return m_ID == other.m_ID;
		}

		virtual bool operator!=(const Asset& other) const
		{
			return !(*this == other);
		}

		UUID m_ID;
	};

}