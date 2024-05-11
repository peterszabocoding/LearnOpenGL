#pragma once

#include "Moongoose/Core.h"
#include "Asset.h"

namespace Moongoose {

	class AssetLoader {
	public:
		virtual Ref<Asset> CreateAsset(AssetDeclaration& decl) = 0;
		virtual Ref<Asset> LoadAsset(AssetDeclaration& declh) = 0;
		virtual void SaveAsset(AssetDeclaration& decl, Ref<Asset> asset) = 0;
	};

	class MeshAssetLoader: public AssetLoader {
	public:
		virtual Ref<Asset> CreateAsset(AssetDeclaration& decl) override;
		virtual Ref<Asset> LoadAsset(AssetDeclaration& decl) override;
		virtual void SaveAsset(AssetDeclaration& decl, Ref<Asset> asset) override;
	};

	class TextureAssetLoader : public AssetLoader {
		virtual Ref<Asset> CreateAsset(AssetDeclaration& decl) override;
		virtual Ref<Asset> LoadAsset(AssetDeclaration& decl) override;
		virtual void SaveAsset(AssetDeclaration& decl, Ref<Asset> asset) override;
	};

	class MaterialAssetLoader : public AssetLoader {
		virtual Ref<Asset> CreateAsset(AssetDeclaration& decl) override;
		virtual Ref<Asset> LoadAsset(AssetDeclaration& decl) override;
		virtual void SaveAsset(AssetDeclaration& decl, Ref<Asset> asset) override;
	};
}