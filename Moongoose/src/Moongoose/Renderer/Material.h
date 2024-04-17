#pragma once

#include "Moongoose/Asset/Asset.h"
#include "Moongoose/Renderer/Texture.h"

namespace Moongoose {

	class Material: public Asset
	{
	public:
		Material() { m_ID = UUID(); }

		virtual AssetType getAssetType() const override { return AssetType::Material; }

		void bind() { Albedo->bind(0); }

	public:
		Ref<Texture2D> Albedo;
	};

}

