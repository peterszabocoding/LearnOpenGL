#pragma once
#include "UUID.h"
#include "AssetTypes.h"

namespace Moongoose {

	class Asset
	{
	public:
		UUID m_ID;

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
	};

}