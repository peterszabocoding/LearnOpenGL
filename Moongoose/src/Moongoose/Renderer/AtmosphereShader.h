#pragma once
#include "Shader.h"
namespace Moongoose {
	class AtmosphereShader : public Shader
	{
		public:
		AtmosphereShader(const std::string& vsLocation, const std::string& fragLocation);
		virtual ~AtmosphereShader() {};



	};
}

