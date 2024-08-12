#include "mgpch.h"
#include "Shader.h"
#include <glm/glm.hpp>

#include "Light.h"
#include "glad/glad.h"
#include "Moongoose/Application.h"

namespace Moongoose {

	Shader::Shader(const ShaderType type, 
		const std::string& vertexShaderLocation, 
		const std::string& fragmentShaderLocation)
	{
		shaderId = 0;
		shaderType = type;

		vertexShaderSourcePath = vertexShaderLocation;
		fragmentShaderSourcePath = fragmentShaderLocation;

		LOG_CORE_INFO("Compile shader: {0} | {1}", vertexShaderSourcePath, fragmentShaderSourcePath);
		CompileShader(
			ReadFile(vertexShaderSourcePath.c_str()).c_str(),
			ReadFile(fragmentShaderSourcePath.c_str()).c_str()
		);
	}

	Shader::Shader(const ShaderType type, 
	               const std::string& vertexShaderLocation, 
	               const std::string& fragmentShaderLocation,
	               const std::string& geometryShaderLocation)
	{
		shaderId = 0;
		shaderType = type;

		vertexShaderSourcePath = vertexShaderLocation;
		fragmentShaderSourcePath = fragmentShaderLocation;
		geometryShaderSourcePath = geometryShaderLocation;

		LOG_CORE_INFO("Compile shader: {0} | {1} | {2}", vertexShaderSourcePath, fragmentShaderSourcePath, geometryShaderSourcePath);
		CompileShader(
			ReadFile(vertexShaderSourcePath.c_str()).c_str(),
			ReadFile(fragmentShaderSourcePath.c_str()).c_str(),
			ReadFile(geometryShaderSourcePath.c_str()).c_str()
		);
	}

	Shader::~Shader()
	{
		ClearShader();
	}

	void Shader::ClearShader()
	{
		if (shaderId != 0) {
			glDeleteProgram(shaderId);
			shaderId = 0;
		}
	}

	ShaderType Shader::GetShaderType() const
	{
		return shaderType;
	}

	void Shader::Bind()
	{
		glPolygonMode(GL_FRONT_AND_BACK, polygonMode == PolygonMode::WIREFRAME ? GL_LINE : GL_FILL);

		if (polygonMode == PolygonMode::WIREFRAME)
		{
			SetLineWidth(2.0f);
			DisableFeature(GlFeature::CULL_FACE);
		} 
		else 
		{
			SetLineWidth(1.0f);
			EnableFeature(GlFeature::DEPTH_TEST);
			EnableFeature(GlFeature::CULL_FACE);
		}
		glUseProgram(shaderId);
	}

	void Shader::Unbind()
	{
		glUseProgram(0);
	}

	std::string Shader::ReadFile(const char* fileLocation)
	{
		std::string content;
		std::ifstream fileStream(fileLocation, std::ios::in);

		if (!fileStream.is_open()) {
			printf("Failed to read %s! File does not exist.", fileLocation);
			return "";
		}

		std::string line;
		while (!fileStream.eof()) {
			std::getline(fileStream, line);
			content.append(line + "\n");
		}

		fileStream.close();
		return content;
	}

	void Shader::SetCamera(const glm::vec3& cameraPosition, const glm::mat4& viewMatrix, const glm::mat4& projection)
	{
		SetMat4("projection", projection);
		SetMat4("view", viewMatrix);
		SetFloat3("eyePosition", cameraPosition);
	}

	void Shader::SetPolygonMode(const PolygonMode mode)
	{
		polygonMode = mode;
	}

	void Shader::SetLineWidth(const float lineWidth)
	{
		glLineWidth(lineWidth);
	}

	void Shader::EnableFeature(GlFeature feature)
	{
		glEnable((int) feature);
	}

	void Shader::DisableFeature(GlFeature feature)
	{
		glDisable((int) feature);
	}	

	void Shader::SetPolygonOffset(const glm::vec2 offset)
	{
		glPolygonOffset(offset.x, offset.y);
	}

	void Shader::SetBlendMode(GlBlendOption source, GlBlendOption destination)
	{
		glBlendFunc((int) source, (int) destination);
	}

	void Shader::BindTexture(const size_t textureUnit, const uint32_t textureId)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}

	void Shader::BindCubeMapTexture(const size_t textureUnit, const uint32_t textureId)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	}

	void Shader::CompileShader(const char* vertexCode, const char* fragmentCode, const char* geometryCode)
	{
		shaderId = glCreateProgram();

		if (!shaderId) {
			LOG_CORE_ERROR("Error creating shader program!");
			return;
		}

		AddShader(shaderId, vertexCode, GL_VERTEX_SHADER);
		AddShader(shaderId, fragmentCode, GL_FRAGMENT_SHADER);

		if (geometryCode) AddShader(shaderId, geometryCode, GL_GEOMETRY_SHADER);

		int result = 0;
		char eLog[1024] = { 0 };

		glLinkProgram(shaderId);
		glGetProgramiv(shaderId, GL_LINK_STATUS, &result);
		if (!result) {
			glGetProgramInfoLog(shaderId, sizeof(eLog), nullptr, eLog);
			printf("Error linking program: '%s'\n", eLog);
			return;
		}

		glValidateProgram(shaderId);
		glGetProgramiv(shaderId, GL_VALIDATE_STATUS, &result);
		if (!result) {
			glGetProgramInfoLog(shaderId, sizeof(eLog), nullptr, eLog);
			printf("Error validating program: '%s'\n", eLog);
		}
	}

	void Shader::AddShader(unsigned int program, const char* shaderCode, unsigned int shaderType) const
	{
		const unsigned int shader = glCreateShader(shaderType);
		const char* code[1];
		code[0] = shaderCode;

		int codeLength[1];
		codeLength[0] = strlen(shaderCode);

		glShaderSource(shader, 1, code, codeLength);
		glCompileShader(shader);

		int result = 0;
		char eLog[1024] = { 0 };

		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		if (!result) {
			glGetShaderInfoLog(shader, sizeof(eLog), nullptr, eLog);
			printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
			return;
		}

		glAttachShader(shaderId, shader);
	}

	unsigned int Shader::GetUniformLocation(const std::string& name)
	{
		unsigned int location;

		if (const auto it = uniformLocationCache.find(name); it != uniformLocationCache.end()) {
			location = it->second;
		}
		else 
		{
			location = glGetUniformLocation(shaderId, name.c_str());
			uniformLocationCache[name] = location;
		}

		return location;
	}

	void Shader::UploadUniformInt(const std::string& name, const int value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetIntArray(const std::string& name, const int* values, const uint32_t count)
	{
		glUniform1iv(GetUniformLocation(name), count, values);
	}

	void Shader::SetFloat(const std::string& name, const float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::SetFloat2(const std::string& name, const glm::vec2& value)
	{
		glUniform2f(GetUniformLocation(name), value.x, value.y);
	}

	void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
	}

	void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void Shader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
	}

}
