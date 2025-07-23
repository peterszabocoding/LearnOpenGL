#include "pch.h"
#include "RenderPass.h"

#include "LearnOpenGL/Renderer/FramebufferManager.h"
#include "LearnOpenGL/Renderer/Renderer.h"
#include "LearnOpenGL/Renderer/ShaderManager.h"

namespace LearnOpenGL
{
	void LightingPass::Render(Ref<Framebuffer> targetBuffer, RenderPassParams& renderPassParams)
	{
		const LightingPassData* data = static_cast<LightingPassData*>(renderPassParams.additionalData);

		targetBuffer->Bind();

		{
			const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::STATIC);
			shader->Bind();

			shader->SetFloat("directionalLight.base.intensity", 0.0f);
			shader->SetFloat3("directionalLight.base.color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
			shader->SetFloat3("directionalLight.direction", glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));

			shader->BindTexture(4, data->shadowMapBuffer->GetShadowMapAttachmentID());
			shader->BindCubeMapTexture(5, data->pointShadowMapBuffer->GetShadowMapCubeAttachmentID());

			for (const auto& light : data->directionalLights)
				AddDirectionalLight(
					light, shader, data->shadowMapBuffer);

			shader->SetInt("pointLightCount", data->pointLights.size());
			for (size_t i = 0; i < data->pointLights.size(); i++) AddPointLight(i, shader, data->pointLights[i]);

			shader->SetInt("spotLightCount", data->spotLights.size());
			for (size_t i = 0; i < data->spotLights.size(); i++) AddSpotLight(i, shader, data->spotLights[i]);

			shader->Unbind();
		}

		for (const auto& cmd : renderPassParams.meshCommands)
		{
			const Ref<Shader> shader = ShaderManager::GetShaderByType(cmd.material->GetShaderType());
			shader->Bind();

			shader->SetCamera(
				renderPassParams.camera->GetCameraPosition(),
				renderPassParams.camera->GetViewMatrix(),
				renderPassParams.camera->GetProjection());

			shader->SetMat4("model", cmd.transform);

			shader->SetInt("material.useAlbedoTexture", cmd.material->m_Albedo != nullptr ? 1 : 0);
			shader->SetInt("material.useMetallicTexture", cmd.material->m_Metallic != nullptr ? 1 : 0);
			shader->SetInt("material.useRoughnessTexture", cmd.material->m_Roughness != nullptr ? 1 : 0);
			shader->SetInt("material.useNormalMapTexture", cmd.material->m_Normal != nullptr ? 1 : 0);

			shader->SetFloat3("material.albedo", cmd.material->m_AlbedoColor);
			shader->SetFloat("material.roughness", cmd.material->m_RoughnessValue);
			shader->SetFloat("material.metallic", cmd.material->m_MetallicValue);

			if (cmd.material) cmd.material->Bind();
			RenderCommand::DrawIndexed(cmd.vertexArray);

			shader->Unbind();
		}

		targetBuffer->Unbind();
	}

	void LightingPass::AddDirectionalLight(
		const DirectionalLight& light, const
		Ref<Shader>& shader,
		const Ref<Framebuffer>& shadowMapBuffer)
	{
		shader->SetFloat3("directionalLight.base.color", light.color);
		shader->SetFloat("directionalLight.base.intensity", light.intensity);
		shader->SetFloat("directionalLight.base.isShadowCasting", light.shadowType != ShadowType::NONE);
		shader->SetFloat("directionalLight.base.useSoftShadow", light.shadowType != ShadowType::NONE);
		shader->SetFloat("directionalLight.base.bias", light.shadowBias);
		shader->SetMat4("directionalLight.base.lightTransform", light.GetTransform());

		shader->SetFloat3("directionalLight.direction", light.direction);
		shader->SetFloat3("directionalLight.ambientColor", light.ambientColor);
		shader->SetFloat("directionalLight.ambientIntensity", light.ambientIntensity);

		shader->BindTexture(4, shadowMapBuffer->GetShadowMapAttachmentID());

		if (light.shadowType != ShadowType::NONE && light.shadowMapRegion)
		{
			const auto& [topLeft, bottomRight] = *light.shadowMapRegion;
			const auto shadowMapSize = glm::vec2(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);

			shader->SetFloat2("directionalLight.base.shadowMapTopLeft", topLeft);
			shader->SetFloat2("directionalLight.base.shadowMapSize", shadowMapSize);
		}
	}

	void LightingPass::AddSpotLight(const size_t index, const Ref<Shader>& shader, const SpotLight& spotLight)
	{
		const std::string base = "spotLights[" + std::to_string(index) + "]";

		shader->SetFloat3(base + ".base.base.color", spotLight.color);
		shader->SetFloat(base + ".base.base.intensity", spotLight.intensity);
		shader->SetFloat(base + ".base.base.isShadowCasting", spotLight.shadowType > ShadowType::NONE);
		shader->SetFloat(base + ".base.base.useSoftShadow", spotLight.shadowType == ShadowType::SOFT);
		shader->SetMat4(base + ".base.base.lightTransform", spotLight.GetTransform());
		shader->SetFloat(base + ".base.base.bias", spotLight.shadowBias);

		shader->SetFloat3(base + ".base.position", spotLight.position);
		shader->SetFloat(base + ".base.attenuationRadius", spotLight.attenuationRadius);

		shader->SetFloat3(base + ".direction", spotLight.direction);
		shader->SetFloat(base + ".attenuationAngle", spotLight.attenuationAngle);

		if (spotLight.shadowType != ShadowType::NONE && spotLight.shadowMapRegion)
		{
			const auto& [topLeft, bottomRight] = *spotLight.shadowMapRegion;
			const auto shadowMapSize = glm::vec2(
				bottomRight.x - topLeft.x,
				bottomRight.y - topLeft.y
			);

			shader->SetFloat2(base + ".base.base.shadowMapTopLeft", topLeft);
			shader->SetFloat2(base + ".base.base.shadowMapSize", shadowMapSize);
		}
	}

	void LightingPass::AddPointLight(const size_t index, const Ref<Shader>& shader, const PointLight& pointLight)
	{
		const std::string base = "pointLights[" + std::to_string(index) + "]";

		shader->SetFloat3(base + ".base.color", pointLight.color);
		shader->SetFloat(base + ".base.intensity", pointLight.intensity);
		shader->SetFloat(base + ".base.isShadowCasting", pointLight.shadowType > ShadowType::NONE);
		shader->SetFloat(base + ".base.useSoftShadow", pointLight.shadowType == ShadowType::SOFT);
		shader->SetFloat(base + ".base.bias", pointLight.shadowBias);

		shader->SetFloat3(base + ".position", pointLight.position);
		shader->SetFloat(base + ".attenuationRadius", pointLight.attenuationRadius);
	}
}
