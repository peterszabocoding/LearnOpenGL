#include "ResourceManager.h"

void ResourceManager::LoadResources()
{
	auto& assetManager = LearnOpenGL::AssetManager::Get();

	m_Icons[Icon::Unknown] = assetManager.CreateAsset<LearnOpenGL::Texture2D>("iconAssetUnknown", "res\\icon\\icon_asset_unknown.png");
	m_Icons[Icon::Mesh] = assetManager.CreateAsset<LearnOpenGL::Texture2D>("iconAssetMesh", "res\\icon\\icon_asset_mesh.png");
	m_Icons[Icon::Texture] = assetManager.CreateAsset<LearnOpenGL::Texture2D>("iconAssetTexture", "res\\icon\\icon_asset_texture.png");
	m_Icons[Icon::Material] = assetManager.CreateAsset<LearnOpenGL::Texture2D>("iconAssetMaterial", "res\\icon\\icon_asset_material.png");
	m_Icons[Icon::DirectionalLight] = assetManager.CreateAsset<LearnOpenGL::Texture2D>("iconDirectionalLight", "res\\icon\\icon_light_directional.png");
	m_Icons[Icon::PointLight] = assetManager.CreateAsset<LearnOpenGL::Texture2D>("iconPointLight", "res\\icon\\icon_light_point.png");
	m_Icons[Icon::FolderClose] = assetManager.CreateAsset<LearnOpenGL::Texture2D>("iconPointLight", "res\\icon\\icon_folder_bw.png");
	m_Icons[Icon::FolderOpen] = assetManager.CreateAsset<LearnOpenGL::Texture2D>("iconPointLight", "res\\icon\\icon_folder_open_bw.png");
	m_Icons[Icon::FolderOpen] = assetManager.CreateAsset<LearnOpenGL::Texture2D>("iconPointLight", "res\\icon\\icon_folder_open_bw.png");
	m_Icons[Icon::EntityCubeBlack] = assetManager.CreateAsset<LearnOpenGL::Texture2D>("iconPointLight", "res\\icon\\icon_entity_cube_lines_black.png");
	m_Icons[Icon::EntityCubeWhite] = assetManager.CreateAsset<LearnOpenGL::Texture2D>("iconPointLight", "res\\icon\\icon_entity_cube_lines_white.png");
}

Ref<LearnOpenGL::Texture2D> ResourceManager::GetIcon(Icon icon)
{
	return m_Icons[icon];
}

std::unordered_map<Icon, Ref<LearnOpenGL::Texture2D>> ResourceManager::m_Icons;