#pragma once

#include <glad/glad.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <tuple>
#include <unordered_map>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "Moongoose/Log.h"

namespace Moongoose {

	enum class ShaderType : uint32_t {
		STATIC = 0,
		LIGHTING,
		STATIC_ALPHA,
		SHADOW_MAP,
		SHADOW_MAP_ALPHA,
		EQUIRECTANGULAR_TO_CUBE,
		IRRADIANCE_MAP,
		BACKGROUND,
		PREFILTER,
		BRDF
	};

	enum class PolygonMode : uint8_t {
		FILL = 0,
		WIREFRAME
	};

	struct ShaderSpecs {
		ShaderType type;
		std::string vertexLocation;
		std::string fragmentLocation;
		PolygonMode polygonMode = PolygonMode::FILL;
	};

	struct UniformDirectionalLight {
		unsigned int uniformColor;
		unsigned int uniformAmbientIntensity;
		unsigned int uniformDiffuseIntensity;
		bool uniformIsShadowCasting;
		bool uniformUseSoftShadow;
		unsigned int uniformDirection;
	};

	struct UniformPointLight {
		unsigned int uniformColor;
		unsigned int uniformAmbientIntensity;
		unsigned int uniformDiffuseIntensity;
		bool uniformIsShadowCasting;
		bool uniformUseSoftShadow;

		unsigned int uniformPosition;
		unsigned int uniformConstant;
		unsigned int uniformLinear;
		unsigned int uniformExponent;
	};

	struct UniformSpotLight {
		unsigned int uniformColor;
		unsigned int uniformAmbientIntensity;
		unsigned int uniformDiffuseIntensity;
		bool uniformIsShadowCasting;
		bool uniformUseSoftShadow;

		unsigned int uniformPosition;
		unsigned int uniformConstant;
		unsigned int uniformLinear;
		unsigned int uniformExponent;

		unsigned int uniformDirection;
		unsigned int uniformAttenuationAngle;
	};

	namespace Utils {
		static std::string GetShaderTypeString(ShaderType type) {
			switch (type) {
			case ShaderType::STATIC:					return "STATIC";
			case ShaderType::STATIC_ALPHA:				return "STATIC_ALPHA";
			case ShaderType::LIGHTING:					return "LIGHTING";
			case ShaderType::SHADOW_MAP:				return "SHADOW_MAP";
			case ShaderType::SHADOW_MAP_ALPHA:			return "SHADOW_MAP_ALPHA";
			case ShaderType::EQUIRECTANGULAR_TO_CUBE:	return "EQUIRECTANGULAR_TO_CUBE";
			case ShaderType::IRRADIANCE_MAP:			return "IRRADIANCE_MAP";
			case ShaderType::BACKGROUND:				return "BACKGROUND";
			case ShaderType::PREFILTER:					return "PREFILTER";
			case ShaderType::BRDF:						return "BRDF";
			}
		}
	}

	class Shader
	{

	public:
		Shader(ShaderSpecs specs);

		std::string ReadFile(const char* fileLocation);

		ShaderType GetShaderType() const { return m_Specs.type; }

		std::string GetShaderTypeString() const { return Utils::GetShaderTypeString(m_Specs.type); }

		unsigned int GetDirectionLocation();

		void SetCamera(const glm::vec3& cameraPosition, const glm::mat4& viewMatrix, const glm::mat4& projection);
		void SetModelTransform(const glm::mat4& model);
		void SetEntityID(const size_t entityId);
		void BindTexture(size_t textureUnit, uint32_t textureID);
		void BindCubeMapTexture(size_t textureUnit, uint32_t textureID);

		void SetDirectionalLight(glm::mat4 transform, glm::vec3 color, float intensity);
		/*
		void SetPointLights(std::vector<std::tuple<Transform*, Ref<Light>>> pLight);
		void SetSpotLights(std::vector<std::tuple<Transform*, Ref<Light>>> sLight);
		
		*/

		void Bind();
		void Unbind();

		void ClearShader();

		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void UploadUniformMat4(const std::string& name, const glm::mat4& value);

		~Shader();

	private:

		ShaderSpecs m_Specs;

		unsigned int shaderID,
			uniformEyePosition,
			uniformTexture,
			uniformNormalMap,
			uniformDirectionalLightTransform, uniformDirectionalShadowMap;

		std::unordered_map<std::string, unsigned int> uniformLocationCache;
		unsigned int uniformPointLightCount;
		unsigned int uniformSpotLightCount;

		UniformDirectionalLight uniformDirectionalLight;
		//UniformPointLight uniformPointLight[MAX_POINT_LIGHTS];
		//UniformSpotLight uniformSpotLight[MAX_SPOT_LIGHTS];

		void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);
		void CompileShader(const char* vertexCode, const char* fragmentCode);
		void AddShader(unsigned int program, const char* shaderCode, unsigned int shaderType);
		unsigned int GetUniformLocation(const std::string& name);
	};

}

