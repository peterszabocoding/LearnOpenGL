#pragma once

#include <string>
#include <fstream>
#include <unordered_map>

#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

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
		BRDF,
		BILLBOARD,
		ATMOSPHERE
	};

	enum class PolygonMode : uint8_t {
		FILL = 0,
		WIREFRAME
	};

	struct UniformDirectionalLight {
		unsigned int uniformColor;
		unsigned int uniformIntensity;
		unsigned int uniformDirection;
		bool uniformIsShadowCasting;
		bool uniformUseSoftShadow;
	};

	struct UniformPointLight {
		unsigned int uniformColor;
		unsigned int uniformIntensity;
		bool uniformIsShadowCasting;
		bool uniformUseSoftShadow;

		unsigned int uniformPosition;
		unsigned int uniformConstant;
		unsigned int uniformLinear;
		unsigned int uniformExponent;
	};

	struct UniformSpotLight {
		unsigned int uniformColor;
		unsigned int uniformIntensity;
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
				case ShaderType::BILLBOARD:					return "BILLBOARD";
				case ShaderType::ATMOSPHERE:				return "ATMOSPHERE";
			}
		}
	}

	class Shader
	{

	public:
		Shader(ShaderType type, const std::string& vertexShaderLocation, const std::string& fragmentShaderLocation);
		virtual ~Shader();
		void ClearShader();
		
		ShaderType GetShaderType() const { return shaderType; }

		void Bind() const;
		void Unbind();
		void BindTexture(size_t textureUnit, uint32_t textureID);
		void BindCubeMapTexture(size_t textureUnit, uint32_t textureId);

		void SetCamera(const glm::vec3& cameraPosition, const glm::mat4& viewMatrix, const glm::mat4& projection);
		void SetModelTransform(const glm::mat4& model);
		void SetEntityId(const size_t entityId);
		void SetPolygonMode(PolygonMode mode);
		void EnablePolygonOffset(bool enable);
		void SetPolygonOffset(glm::vec2 offset);
		void SetDepthTest(bool enabled);
		void SetBlendMode(bool enabled);

		void ResetLights();

		std::string GetShaderTypeString() const { return Utils::GetShaderTypeString(shaderType); }

		unsigned int GetDirectionLocation() const;
		void SetDirectionalLight(glm::vec3 direction, glm::vec3 color, const float intensity, bool isShadowCasting, bool useSoftShadow);
		void SetPointLight(glm::vec3 position, glm::vec3 color, float intensity, float attenuationRadius);
		void SetSpotLight(glm::vec3 direction, glm::vec3 position, glm::vec3 color, float intensity, float attenuationRadius, float attenuationAngle);
		
		/*
		void SetPointLights(std::vector<std::tuple<Transform*, Ref<Light>>> pLight);
		void SetSpotLights(std::vector<std::tuple<Transform*, Ref<Light>>> sLight);
		*/
		void UploadUniformInt(const std::string& name, int value);
		void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
		void UploadUniformFloat(const std::string& name, float value);
		void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
		void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
		void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
		void UploadUniformMat4(const std::string& name, const glm::mat4& value);

	private:
		std::string ReadFile(const char* fileLocation);
		void CompileShader(const char* vertexCode, const char* fragmentCode);
		void AddShader(unsigned int program, const char* shaderCode, unsigned int shaderType);
		unsigned int GetUniformLocation(const std::string& name);

	private:
		ShaderType shaderType;
		std::string vertexShaderSourcePath;
		std::string fragmentShaderSourcePath;
		PolygonMode polygonMode = PolygonMode::FILL;

		unsigned int shaderID;
		unsigned int uniformEyePosition;
		unsigned int uniformTexture;
		unsigned int uniformNormalMap;
		unsigned int uniformDirectionalLightTransform;
		unsigned int uniformDirectionalShadowMap;
		unsigned int uniformPointLightCount;
		unsigned int uniformSpotLightCount;

		std::unordered_map<std::string, unsigned int> uniformLocationCache;

		UniformDirectionalLight uniformDirectionalLight;
		UniformPointLight uniformPointLight[100];
		UniformSpotLight uniformSpotLight[100];
	};

}

