#include "mgpch.h"
#include "Shader.h"

namespace Moongoose {

	Shader::Shader(ShaderSpecs specs)
	{
		shaderID = 0;
		m_Specs = specs;
		CreateFromFiles(m_Specs.vertexLocation.c_str(), m_Specs.fragmentLocation.c_str());
	}

	Shader::~Shader()
	{
		ClearShader();
	}

	void Shader::CreateFromFiles(const char* vertexLocation, const char* fragmentLocation)
	{
		std::string vertexString = ReadFile(vertexLocation);
		std::string fragmentString = ReadFile(fragmentLocation);

		LOG_CORE_INFO("Compile shader: {0} | {1}", vertexLocation, fragmentLocation);
		CompileShader(vertexString.c_str(), fragmentString.c_str());
	}

	std::string Shader::ReadFile(const char* fileLocation)
	{
		std::string content;
		std::ifstream fileStream(fileLocation, std::ios::in);

		if (!fileStream.is_open()) {
			printf("Failed to read %s! File does not exist.", fileLocation);
			return "";
		}

		std::string line = "";
		while (!fileStream.eof()) {
			std::getline(fileStream, line);
			content.append(line + "\n");
		}

		fileStream.close();

		return content;
	}

	GLuint Shader::GetDirectionLocation()
	{
		return uniformDirectionalLight.uniformDirection;
	}

	void Shader::SetCamera(const glm::vec3& cameraPosition, const glm::mat4& viewMatrix, const glm::mat4& projection)
	{
		UploadUniformMat4("projection", projection);
		UploadUniformMat4("view", viewMatrix);
		UploadUniformFloat3("eyePosition", cameraPosition);
	}

	void Shader::SetModelTransform(const glm::mat4& model)
	{
		UploadUniformMat4("model", model);
	}

	void Shader::BindTexture(size_t textureUnit, uint32_t textureID)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}

	void Shader::BindCubeMapTexture(size_t textureUnit, uint32_t textureID)
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	}

	/*
	void Shader::SetDirectionalLight(const std::tuple<Transform*, Ref<Light>>& directionalLightTuple)
	{
		const Transform* transform = std::get<0>(directionalLightTuple);
		const Ref<Light>& dirLight = std::get<1>(directionalLightTuple);
		const auto& lightTransform = dirLight->CalculateLightTransform(transform);

		glUniformMatrix4fv(uniformDirectionalLightTransform, 1, false, glm::value_ptr(lightTransform));

		UploadUniformFloat3("directionalLight.base.color", dirLight->GetColor());
		UploadUniformFloat("directionalLight.base.ambientIntensity", dirLight->GetAmbientIntensity());
		UploadUniformFloat("directionalLight.base.diffuseIntensity", dirLight->GetDiffuseIntensity());
		UploadUniformFloat("directionalLight.base.isShadowCasting", dirLight->IsShadowCasting());
		UploadUniformFloat("directionalLight.base.useSoftShadow", dirLight->UseSoftShadow());
		UploadUniformFloat3("directionalLight.direction", dirLight->GetDirection(transform));
	}

	void Shader::SetPointLights(std::vector<std::tuple<Transform*, Ref<Light>>> pLight)
	{
		size_t lightCount = pLight.size();
		if (lightCount > MAX_POINT_LIGHTS) lightCount = MAX_POINT_LIGHTS;

		glUniform1i(uniformPointLightCount, lightCount);

		for (size_t i = 0; i < lightCount; i++) {
			const auto& pointLight = std::get<1>(pLight[i]);
			const auto& position = std::get<0>(pLight[i])->position;
			const auto& color = pointLight->GetColor();

			glUniform3f(uniformPointLight[i].uniformColor, color.x, color.y, color.z);
			glUniform1f(uniformPointLight[i].uniformAmbientIntensity, pointLight->GetAmbientIntensity());
			glUniform1f(uniformPointLight[i].uniformDiffuseIntensity, pointLight->GetDiffuseIntensity());

			glUniform1f(uniformPointLight[i].uniformIsShadowCasting, pointLight->IsShadowCasting());
			glUniform1f(uniformPointLight[i].uniformUseSoftShadow, pointLight->UseSoftShadow());

			glUniform3f(uniformPointLight[i].uniformPosition, position.x, position.y, position.z);

			glUniform1f(uniformPointLight[i].uniformConstant, pointLight->GetAttenuationParams().constant);
			glUniform1f(uniformPointLight[i].uniformLinear, pointLight->GetAttenuationParams().linear);
			glUniform1f(uniformPointLight[i].uniformExponent, pointLight->GetAttenuationParams().exponent);
		}
	}

	void Shader::SetSpotLights(std::vector<std::tuple<Transform*, Ref<Light>>> sLight)
	{
		size_t lightCount = sLight.size();
		if (lightCount > MAX_SPOT_LIGHTS) lightCount = MAX_SPOT_LIGHTS;

		glUniform1i(uniformSpotLightCount, lightCount);

		for (size_t i = 0; i < lightCount; i++) {
			Transform* transform = std::get<0>(sLight[i]);
			auto& spotLight = std::get<1>(sLight[i]);

			auto& color = spotLight->GetColor();
			auto direction = spotLight->GetDirection(transform);

			glUniform3f(uniformSpotLight[i].uniformColor, color.x, color.y, color.z);
			glUniform1f(uniformSpotLight[i].uniformAmbientIntensity, spotLight->GetAmbientIntensity());
			glUniform1f(uniformSpotLight[i].uniformDiffuseIntensity, spotLight->GetDiffuseIntensity());

			glUniform1f(uniformSpotLight[i].uniformIsShadowCasting, spotLight->IsShadowCasting());
			glUniform1f(uniformSpotLight[i].uniformUseSoftShadow, spotLight->UseSoftShadow());

			glUniform3f(uniformSpotLight[i].uniformPosition, transform->position.x, transform->position.y, transform->position.z);

			glUniform1f(uniformSpotLight[i].uniformConstant, spotLight->GetAttenuationParams().constant);
			glUniform1f(uniformSpotLight[i].uniformLinear, spotLight->GetAttenuationParams().linear);
			glUniform1f(uniformSpotLight[i].uniformExponent, spotLight->GetAttenuationParams().exponent);

			glUniform1f(uniformSpotLight[i].uniformAttenuationAngle, spotLight->GetAttenuationAngle());
			glUniform3f(uniformSpotLight[i].uniformDirection, direction.x, direction.y, direction.z);
		}
	}
	*/

	void Shader::Bind()
	{
		glUseProgram(shaderID);
	}

	void Shader::Unbind()
	{
		glUseProgram(0);
	}

	void Shader::ClearShader()
	{
		if (shaderID != 0) {
			glDeleteProgram(shaderID);
			shaderID = 0;
		}
	}

	void Shader::CompileShader(const char* vertexCode, const char* fragmentCode)
	{
		shaderID = glCreateProgram();

		if (!shaderID) {
			LOG_CORE_ERROR("Error creating shader program!");
			return;
		}

		AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
		AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

		GLint result = 0;
		GLchar eLog[1024] = { 0 };

		glLinkProgram(shaderID);
		glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
		if (!result) {
			glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
			printf("Error linking program: '%s'\n", eLog);
			return;
		}

		glValidateProgram(shaderID);
		glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
		if (!result) {
			glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
			printf("Error validating program: '%s'\n", eLog);
			return;
		}

		/*

		uniformPointLightCount = glGetUniformLocation(shaderID, "pointLightCount");
		for (size_t i = 0; i < MAX_POINT_LIGHTS; i++) {
			char locBuff[100] = { '\0' };

			snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.color", i);
			uniformPointLight[i].uniformColor = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.ambientIntensity", i);
			uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.diffuseIntensity", i);
			uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.isShadowCasting", i);
			uniformPointLight[i].uniformIsShadowCasting = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "pointLights[%d].base.useSoftShadow", i);
			uniformPointLight[i].uniformUseSoftShadow = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "pointLights[%d].position", i);
			uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "pointLights[%d].constant", i);
			uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "pointLights[%d].linear", i);
			uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "pointLights[%d].exponent", i);
			uniformPointLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);
		}

		uniformSpotLightCount = glGetUniformLocation(shaderID, "spotLightCount");
		for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++) {
			char locBuff[100] = { '\0' };

			snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.color", i);
			uniformSpotLight[i].uniformColor = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.ambientIntensity", i);
			uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.diffuseIntensity", i);
			uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.isShadowCasting", i);
			uniformSpotLight[i].uniformIsShadowCasting = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.useSoftShadow", i);
			uniformSpotLight[i].uniformUseSoftShadow = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.position", i);
			uniformSpotLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.constant", i);
			uniformSpotLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.linear", i);
			uniformSpotLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.exponent", i);
			uniformSpotLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "spotLights[%d].direction", i);
			uniformSpotLight[i].uniformDirection = glGetUniformLocation(shaderID, locBuff);

			snprintf(locBuff, sizeof(locBuff), "spotLights[%d].attenuationAngle", i);
			uniformSpotLight[i].uniformAttenuationAngle = glGetUniformLocation(shaderID, locBuff);
		}

		*/

		uniformTexture = glGetUniformLocation(shaderID, "AlbedoTexture");
		uniformNormalMap = glGetUniformLocation(shaderID, "NormalTexture");
		uniformDirectionalLightTransform = glGetUniformLocation(shaderID, "directionalLightTransform");
		uniformDirectionalShadowMap = glGetUniformLocation(shaderID, "directionalShadowMap");
	}

	void Shader::AddShader(GLuint program, const char* shaderCode, GLenum shaderType)
	{
		GLuint shader = glCreateShader(shaderType);
		const GLchar* code[1];
		code[0] = shaderCode;

		GLint codeLength[1];
		codeLength[0] = strlen(shaderCode);

		glShaderSource(shader, 1, code, codeLength);
		glCompileShader(shader);

		GLint result = 0;
		GLchar eLog[1024] = { 0 };

		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		if (!result) {
			glGetShaderInfoLog(shader, sizeof(eLog), NULL, eLog);
			printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
			return;
		}

		glAttachShader(shaderID, shader);
	}

	GLuint Shader::GetUniformLocation(const std::string& name)
	{
		GLuint location;

		auto it = uniformLocationCache.find(name);
		if (it != uniformLocationCache.end()) {
			location = it->second;
		}
		else {
			location = glGetUniformLocation(shaderID, name.c_str());
			uniformLocationCache[name] = location;
		}

		return location;
	}

	void Shader::UploadUniformInt(const std::string& name, int value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count)
	{
		glUniform1iv(GetUniformLocation(name), count, values);
	}

	void Shader::UploadUniformFloat(const std::string& name, float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& value)
	{
		glUniform2f(GetUniformLocation(name), value.x, value.y);
	}

	void Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& value)
	{
		glUniform3f(GetUniformLocation(name), value.x, value.y, value.z);
	}

	void Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& value)
	{
		glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void Shader::UploadUniformMat4(const std::string& name, const glm::mat4& value)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
	}


}