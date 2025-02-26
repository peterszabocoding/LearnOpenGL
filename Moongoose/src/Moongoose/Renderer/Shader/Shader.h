#pragma once

#include <string>
#include <fstream>
#include <unordered_map>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Moongoose/Log.h"

namespace Moongoose
{
	enum class ShaderType : uint32_t
	{
		NONE = 0,
		STATIC,
		LIGHTING,
		STATIC_ALPHA,
		SHADOW_MAP_DIRECTIONAL,
		SHADOW_MAP_POINT,
		SHADOW_MAP_SPOT,
		SHADOW_MAP_ALPHA,
		EQUIRECTANGULAR_TO_CUBE,
		IRRADIANCE_MAP,
		BACKGROUND,
		PREFILTER,
		BRDF,
		BILLBOARD,
		ATMOSPHERE,
		GBUFFER,
		POST_PROCESS_COMBINE,
		POST_PROCESS_SSR,
		POST_PROCESS_BOX_BLUR,
	};

	enum class PolygonMode : uint8_t
	{
		FILL = 0,
		WIREFRAME
	};

	constexpr size_t MAX_POINT_LIGHTS = 4;
	constexpr size_t MAX_SPOT_LIGHTS = 4;

	enum class GlFeature : int
	{
		DEPTH_TEST = GL_DEPTH_TEST,
		POLYGON_OFFSET = GL_POLYGON_OFFSET_FILL,
		BLEND = GL_BLEND,
		CULL_FACE = GL_CULL_FACE,
		FRAMEBUFFER_SRGB = GL_FRAMEBUFFER_SRGB
	};

	enum class GlBlendOption : int
	{
		SRC_ALPHA = GL_SRC_ALPHA,
		ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA
	};

	struct UniformDirectionalLight
	{
		unsigned int uniformColor;
		unsigned int uniformIntensity;
		unsigned int uniformDirection;
		bool uniformIsShadowCasting;
		bool uniformUseSoftShadow;
	};

	struct UniformPointLight
	{
		unsigned int uniformColor;
		unsigned int uniformIntensity;
		bool uniformIsShadowCasting;
		bool uniformUseSoftShadow;

		unsigned int uniformPosition;
	};

	struct UniformSpotLight
	{
		unsigned int uniformColor;
		unsigned int uniformIntensity;
		bool uniformIsShadowCasting;
		bool uniformUseSoftShadow;

		unsigned int uniformPosition;
		unsigned int uniformDirection;
		unsigned int uniformAttenuationAngle;
	};

	namespace Utils
	{
		static std::string GetShaderTypeString(ShaderType type)
		{
			switch (type)
			{
			case ShaderType::STATIC: return "STATIC";
			case ShaderType::STATIC_ALPHA: return "STATIC_ALPHA";
			case ShaderType::LIGHTING: return "LIGHTING";
			case ShaderType::SHADOW_MAP_DIRECTIONAL: return "SHADOW_MAP_DIRECTIONAL";
			case ShaderType::SHADOW_MAP_POINT: return "SHADOW_MAP_POINT";
			case ShaderType::SHADOW_MAP_ALPHA: return "SHADOW_MAP_ALPHA";
			case ShaderType::EQUIRECTANGULAR_TO_CUBE: return "EQUIRECTANGULAR_TO_CUBE";
			case ShaderType::IRRADIANCE_MAP: return "IRRADIANCE_MAP";
			case ShaderType::BACKGROUND: return "BACKGROUND";
			case ShaderType::PREFILTER: return "PREFILTER";
			case ShaderType::BRDF: return "BRDF";
			case ShaderType::BILLBOARD: return "BILLBOARD";
			case ShaderType::ATMOSPHERE: return "ATMOSPHERE";
			}
		}
	}

	class Shader
	{
	public:
		struct ShaderParams
		{
			ShaderType type = ShaderType::NONE;
			std::string vertexShaderFilePath;
			std::string fragmentShaderFilePath;
			std::string geometryShaderFilePath;
		};

	public:
		Shader(const ShaderParams& params);
		virtual ~Shader();

		void ClearShader();

		[[nodiscard]] ShaderType GetShaderType() const;

		void Bind();
		void Unbind();
		void BindTexture(size_t textureUnit, uint32_t textureId);
		void BindCubeMapTexture(size_t textureUnit, uint32_t textureId);

		void SetCamera(const glm::vec3& cameraPosition, const glm::mat4& viewMatrix, const glm::mat4& projection);

		void EnableFeature(GlFeature feature);
		void DisableFeature(GlFeature feature);

		void SetPolygonMode(PolygonMode mode);
		void SetLineWidth(float lineWidth);
		void SetPolygonOffset(glm::vec2 offset);
		void SetBlendMode(GlBlendOption source, GlBlendOption destination);

		[[nodiscard]] std::string GetShaderTypeString() const { return Utils::GetShaderTypeString(shaderType); }

		void SetInt(const std::string& name, int value);
		void SetIntArray(const std::string& name, const int* values, uint32_t count);
		void SetFloat(const std::string& name, float value);
		void SetFloat2(const std::string& name, const glm::vec2& value);
		void SetFloat3(const std::string& name, const glm::vec3& value);
		void SetFloat4(const std::string& name, const glm::vec4& value);
		void SetMat4(const std::string& name, const glm::mat4& value);

	private:
		static std::string ReadFile(const char* fileLocation);
		void CompileShader(const char* vertexCode, const char* fragmentCode, const char* geometryCode = nullptr);
		void AddShader(unsigned int program, const char* shaderCode, unsigned int shaderType) const;
		unsigned int GetUniformLocation(const std::string& name);

	private:
		unsigned int shaderId;

		ShaderType shaderType;
		std::string vertexShaderSourcePath;
		std::string fragmentShaderSourcePath;
		std::string geometryShaderSourcePath;
		PolygonMode polygonMode = PolygonMode::FILL;

		std::unordered_map<std::string, unsigned int> uniformLocationCache;
	};
}
