#include "mgpch.h"
#include "ShaderManager.h"

namespace Moongoose {

	std::unordered_map<ShaderType, Ref<Shader>> ShaderManager::s_ShadersByType;

	void ShaderManager::AssignShaderToType(ShaderType type, 
		const std::string& vertexShaderLocation, 
		const std::string& fragmentShaderLocation,
		const std::string& geometryShaderLocation)
	{
		if (s_ShadersByType[type]) s_ShadersByType[type]->ClearShader();

		Ref<Shader> shader;
		if (!geometryShaderLocation.empty())
		{
			shader = CreateRef<Shader>(type, vertexShaderLocation, fragmentShaderLocation, geometryShaderLocation);
		} else
		{
			shader = CreateRef<Shader>(type, vertexShaderLocation, fragmentShaderLocation);
		}

		s_ShadersByType[type] = shader;
	}

	Ref<Shader> ShaderManager::GetShaderByType(const ShaderType type)
	{
		return s_ShadersByType[type];
	}
}