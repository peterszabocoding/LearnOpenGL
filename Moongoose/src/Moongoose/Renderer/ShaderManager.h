#pragma once

#include "Moongoose/Renderer/Shader.h"

namespace Moongoose {

	class ShaderManager
	{
	public:
		static ShaderManager& Get()
		{
			static ShaderManager manager;
			return manager;
		}

		static void AssignShaderToType(ShaderType type, const std::string& vertexShaderLocation, const std::string& fragmentShaderLocation);
		static Ref<Shader> GetShaderByType(ShaderType type);

	private:
		static std::unordered_map<ShaderType, Ref<Shader>> s_ShadersByType;
	};
}

