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
		ALPHA_TEST,
		SHADOW_MAP,
		SHADOW_MAP_ALPHA,
		EQUIRECTANGULAR_TO_CUBE,
		IRRADIANCE_MAP,
		BACKGROUND,
		PREFILTER,
		BRDF
	};

	struct ShaderSpecs {
		ShaderType type;
		std::string vertexLocation;
		std::string fragmentLocation;
	};

	struct UniformDirectionalLight {
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;
		GLboolean uniformIsShadowCasting;
		GLboolean uniformUseSoftShadow;

		GLuint uniformDirection;
	};

	struct UniformPointLight {
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;
		GLboolean uniformIsShadowCasting;
		GLboolean uniformUseSoftShadow;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
	};

	struct UniformSpotLight {
		GLuint uniformColor;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;
		GLboolean uniformIsShadowCasting;
		GLboolean uniformUseSoftShadow;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;

		GLuint uniformDirection;
		GLuint uniformAttenuationAngle;
	};

	namespace Utils {
		static std::string GetShaderTypeString(ShaderType type) {
			switch (type) {
			case ShaderType::STATIC:					return "Static";
			case ShaderType::ALPHA_TEST:				return "Static - Alpha tested";
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

		ShaderType GetShaderType() const {
			return m_Specs.type;
		}

		std::string GetShaderTypeString() const {
			return Utils::GetShaderTypeString(m_Specs.type);
		}

		GLuint GetDirectionLocation();

		void SetCamera(const glm::vec3& cameraPosition, const glm::mat4& viewMatrix, const glm::mat4& projection);
		void SetModelTransform(const glm::mat4& model);
		void BindTexture(size_t textureUnit, uint32_t textureID);
		void BindCubeMapTexture(size_t textureUnit, uint32_t textureID);

		/*
		
		void SetDirectionalLight(const std::tuple<Transform*, Ref<Light>>& directionalLightTuple);
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

		GLuint shaderID,
			uniformEyePosition,
			uniformTexture,
			uniformNormalMap,
			uniformDirectionalLightTransform, uniformDirectionalShadowMap;

		std::unordered_map<std::string, GLuint> uniformLocationCache;
		GLuint uniformPointLightCount;
		GLuint uniformSpotLightCount;

		UniformDirectionalLight uniformDirectionalLight;
		//UniformPointLight uniformPointLight[MAX_POINT_LIGHTS];
		//UniformSpotLight uniformSpotLight[MAX_SPOT_LIGHTS];

		void CreateFromFiles(const char* vertexLocation, const char* fragmentLocation);
		void CompileShader(const char* vertexCode, const char* fragmentCode);
		void AddShader(GLuint program, const char* shaderCode, GLenum shaderType);
		GLuint GetUniformLocation(const std::string& name);
	};

}

