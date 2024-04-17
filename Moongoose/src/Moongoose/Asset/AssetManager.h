#pragma once

#include "Moongoose/Core.h"
#include "Moongoose/Renderer/Mesh.h"
#include "Moongoose/Renderer/Texture.h"
#include "Moongoose/Renderer/Shader.h"

namespace Moongoose {

	class AssetManager
	{
	public:
		static Ref<Mesh> LoadMesh(std::string meshPath);
		static Ref<Texture2D> LoadTexture2D(const std::string& filepath, TextureFormat textureFormat);
		static Ref<Shader> LoadShader(ShaderType shaderType = ShaderType::STATIC);
	};

}