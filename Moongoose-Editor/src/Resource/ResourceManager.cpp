#include "ResourceManager.h"

void ResourceManager::LoadResources()
{
	auto& assetManager = Moongoose::AssetManager::Get();

	m_Icons[Icon::Unknown] = assetManager.CreateAsset<Moongoose::Texture2D>("iconAssetUnknown", "res\\icon\\icon_asset_unknown.png");
	m_Icons[Icon::Mesh] = assetManager.CreateAsset<Moongoose::Texture2D>("iconAssetMesh", "res\\icon\\icon_asset_mesh.png");
	m_Icons[Icon::Texture] = assetManager.CreateAsset<Moongoose::Texture2D>("iconAssetTexture", "res\\icon\\icon_asset_texture.png");
	m_Icons[Icon::Material] = assetManager.CreateAsset<Moongoose::Texture2D>("iconAssetMaterial", "res\\icon\\icon_asset_material.png");
	m_Icons[Icon::Directional_Light] = assetManager.CreateAsset<Moongoose::Texture2D>("iconDirectionalLight", "res\\icon\\icon_light_directional.png");
}

Ref<Moongoose::Texture2D> ResourceManager::GetIcon(Icon icon)
{
	return m_Icons[icon];
}

std::unordered_map<Icon, Ref<Moongoose::Texture2D>> ResourceManager::m_Icons;