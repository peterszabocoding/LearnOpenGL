#pragma once

#include "Moongoose/Core.h"
#include "Asset.h"

namespace Moongoose {

	class AssetLoader {
	public:
		virtual ~AssetLoader() = default;
		virtual Ref<Asset> CreateAsset(AssetDeclaration& decl) = 0;
		virtual Ref<Asset> LoadAsset(AssetDeclaration& decl) = 0;
		virtual Ref<Asset> LoadAssetFromFile(AssetDeclaration& decl) = 0;
		virtual Ref<Asset> ReloadAsset(AssetDeclaration& decl) = 0;
		virtual Ref<Asset> GetDefaultAsset() = 0;
		virtual void SaveAsset(Ref<Asset> asset) = 0;
	};

	class MeshAssetLoader: public AssetLoader {
	public:
		virtual Ref<Asset> CreateAsset(AssetDeclaration& decl) override;
		virtual Ref<Asset> LoadAsset(AssetDeclaration& decl) override;
		virtual Ref<Asset> LoadAssetFromFile(AssetDeclaration& decl) override;
		virtual Ref<Asset> ReloadAsset(AssetDeclaration& decl) override;
		virtual Ref<Asset> GetDefaultAsset() override;
		virtual void SaveAsset(Ref<Asset> asset) override;
	};

	class TextureAssetLoader: public AssetLoader {
	public:
		virtual Ref<Asset> CreateAsset(AssetDeclaration& decl) override;
		virtual Ref<Asset> LoadAsset(AssetDeclaration& decl) override;
		virtual Ref<Asset> LoadAssetFromFile(AssetDeclaration& decl) override;
		virtual Ref<Asset> ReloadAsset(AssetDeclaration& decl) override;
		virtual Ref<Asset> GetDefaultAsset() override;
		virtual void SaveAsset(Ref<Asset> asset) override;
	};

	class MaterialAssetLoader: public AssetLoader {
	public:
		virtual Ref<Asset> CreateAsset(AssetDeclaration& decl) override;
		virtual Ref<Asset> LoadAsset(AssetDeclaration& decl) override;
		virtual Ref<Asset> LoadAssetFromFile(AssetDeclaration& decl) override;
		virtual Ref<Asset> ReloadAsset(AssetDeclaration& decl) override;
		virtual Ref<Asset> GetDefaultAsset() override;
		virtual void SaveAsset(Ref<Asset> asset) override;
	};
}