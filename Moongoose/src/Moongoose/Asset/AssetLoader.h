#pragma once

#include "Moongoose/Core.h"
#include "Asset.h"

namespace Moongoose {

	class AssetLoader {
	public:
		virtual Ref<Asset> LoadAsset(const std::string& filePath) = 0;
	};

	class MeshAssetLoader: public AssetLoader {
	public:
		virtual Ref<Asset> LoadAsset(const std::string& filePath) override;
	};

	class TextureAssetLoader : public AssetLoader {
		virtual Ref<Asset> LoadAsset(const std::string& filePath) override;
	};
}