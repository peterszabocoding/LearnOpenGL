#include "mgpch.h"
#include "AtmosphereShader.h"

namespace Moongoose {

	AtmosphereShader::AtmosphereShader(const std::string& vsLocation, const std::string& fragLocation): Shader(ShaderType::ATMOSPHERE, vsLocation, fragLocation)
	{
	}

}
