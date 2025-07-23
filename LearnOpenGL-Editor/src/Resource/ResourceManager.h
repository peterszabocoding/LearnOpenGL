#pragma once
#include <LearnOpenGL.h>

enum class Icon : uint16_t
{
	Unknown = 0,
	Scene,
	Mesh,
	Material,
	Texture,
	EnvMap,
	DirectionalLight,
	PointLight,
	FolderOpen,
	FolderClose,
	EntityCubeWhite,
	EntityCubeBlack
};

class ResourceManager
{
public:
	static void LoadResources();
	static Ref<LearnOpenGL::Texture2D> GetIcon(Icon icon);

private:
	static std::unordered_map<Icon, Ref<LearnOpenGL::Texture2D>> m_Icons;
};

