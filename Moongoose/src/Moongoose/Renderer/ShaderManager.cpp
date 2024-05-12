#include "mgpch.h"
#include "ShaderManager.h"

namespace Moongoose {

	std::unordered_map<ShaderType, Ref<Shader>> ShaderManager::s_ShadersByType;
	
	void ShaderManager::AssignShaderToType(ShaderType type, const std::string& vertexShaderLocation, const std::string& fragmentShaderLocation)
	{
		if (s_ShadersByType[type]) s_ShadersByType[type]->ClearShader();

		auto shader = CreateRef<Shader>(type, vertexShaderLocation, fragmentShaderLocation);
		s_ShadersByType[type] = shader;
	}

	Ref<Shader> ShaderManager::GetShaderByType(ShaderType type)
	{
		return s_ShadersByType[type];
	}
}