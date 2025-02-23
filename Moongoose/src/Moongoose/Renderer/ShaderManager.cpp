#include "mgpch.h"
#include "ShaderManager.h"

namespace Moongoose
{
	std::unordered_map<ShaderType, Ref<Shader>> ShaderManager::s_ShadersByType;

	void ShaderManager::AssignShaderToType(const ShaderType type,
	                                       const std::string& vsLocation,
	                                       const std::string& fsLocation,
	                                       const std::string& geoLocation)
	{
		if (s_ShadersByType[type]) s_ShadersByType[type]->ClearShader();

		Ref<Shader> shader;
		if (!geoLocation.empty())
		{
			shader = CreateRef<Shader>(Shader::ShaderParams{
				type, vsLocation, fsLocation, geoLocation
			});
		}
		else
		{
			shader = CreateRef<Shader>(Shader::ShaderParams{type, vsLocation, fsLocation});
		}

		s_ShadersByType[type] = shader;
	}

	Ref<Shader> ShaderManager::GetShaderByType(const ShaderType type)
	{
		return s_ShadersByType[type];
	}
}
