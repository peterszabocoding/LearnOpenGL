#pragma once

#include "Moongoose/Renderer/Shader/Shader.h"

namespace Moongoose
{
	class ShaderManager
	{
	public:
		static ShaderManager& Get()
		{
			static ShaderManager manager;
			return manager;
		}

		static void AssignShaderToType(ShaderType type,
		                               const std::string& vsLocation,
		                               const std::string& fsLocation,
		                               const std::string& geoLocation);

		static Ref<Shader> GetShaderByType(ShaderType type);

	private:
		static std::unordered_map<ShaderType, Ref<Shader>> s_ShadersByType;
	};
}
