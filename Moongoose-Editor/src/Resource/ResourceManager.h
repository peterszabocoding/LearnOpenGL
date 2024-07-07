#pragma once
#include <Moongoose.h>

enum class Icon : uint16_t
{
	Unknown = 0,
	Scene = 1,
	Mesh = 2,
	Material = 3,
	Texture = 4,
	EnvMap = 5,
};

class ResourceManager
{
public:
	static void LoadResources();

	static Ref<Moongoose::Texture2D> GetIcon(Icon icon);

private:
	static std::unordered_map<Icon, Ref<Moongoose::Texture2D>> m_Icons;
};

