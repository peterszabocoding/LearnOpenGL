#include "mgpch.h"
#include "Renderer.h"

#include "FramebufferManager.h"
#include "MeshPrimitives.h"
#include "RenderCommand.h"
#include "ShaderManager.h"
#include "Moongoose/ECS/World.h"
#include "Moongoose/ECS/Systems/AtmosphericsSystem.h"
#include "Moongoose/ECS/Systems/BillboardSystem.h"
#include "Moongoose/ECS/Systems/LightSystem.h"
#include "Moongoose/ECS/Systems/RenderSystem.h"
#include "Moongoose/Renderer/Framebuffer.h"
#include "Light.h"

namespace Moongoose {

	glm::uvec2 Renderer::m_Resolution;
	Ref<Framebuffer> Renderer::m_RenderBuffer;
	Ref<Framebuffer> Renderer::m_ShadowBuffer;

	TextureAtlas								Renderer::m_TextureAtlas;

	std::vector<DirectionalLight>				Renderer::m_DirectionalLights;
	std::vector<PointLight>						Renderer::m_PointLights;
	std::vector<SpotLight>						Renderer::m_SpotLights;

	std::vector<Renderer::MeshRenderCmd>		Renderer::m_MeshRenderCmds;
	std::vector<Renderer::BillboardCmd>			Renderer::m_BillboardRenderCmds;

	void Renderer::SetResolution(const glm::uvec2 newResolution)
	{
		if (!m_ShadowBuffer) InitShadowBuffer();
		if (newResolution == m_Resolution) return;

		FramebufferSpecs specs;
		specs.Width = newResolution.x;
		specs.Height = newResolution.y;
		specs.Attachments = {
			FramebufferTextureFormat::RGBA8,
			FramebufferTextureFormat::RED_INTEGER,
			FramebufferTextureFormat::DEPTH24STENCIL8
		};
		specs.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		m_RenderBuffer = FramebufferManager::CreateFramebuffer("RenderBuffer", specs);
		m_Resolution = newResolution;
	}

	void Renderer::PrepareLights()
	{
		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::STATIC);
		shader->Bind();

		shader->UploadUniformInt("spotLightCount", 0);
		shader->UploadUniformInt("pointLightCount", 0);

		shader->UploadUniformFloat("directionalLight.base.intensity", 0.0f);
		shader->UploadUniformFloat3("directionalLight.base.color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		shader->UploadUniformFloat3("directionalLight.direction", glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));

		shader->BindTexture(4, m_ShadowBuffer->GetShadowMapAttachmentID());

		for (const auto& light : m_DirectionalLights) PrepareDirectionalLight(light, shader);

		SetPointLightCount(shader, m_PointLights.size());
		for (size_t i = 0; i < m_PointLights.size(); i++) AddPointLight(i, shader, m_PointLights[i], GetPointLightTransform(m_PointLights[i]));

		SetSpotLightCount(shader, m_SpotLights.size());
		for (size_t i = 0; i < m_SpotLights.size(); i++)
		{
			AddSpotLight(i, shader, m_SpotLights[i], GetSpotLightTransform(m_SpotLights[i]));
		}
		shader->Unbind();
	}

	void Renderer::PrepareDirectionalLight(const DirectionalLight& light, const Ref<Shader>& shader)
	{
		shader->UploadUniformFloat3("directionalLight.base.color", light.color);
		shader->UploadUniformFloat("directionalLight.base.intensity", light.intensity);
		shader->UploadUniformFloat("directionalLight.base.isShadowCasting", light.shadowType != ShadowType::NONE);
		shader->UploadUniformFloat("directionalLight.base.useSoftShadow", light.shadowType != ShadowType::NONE);
		shader->UploadUniformMat4("directionalLight.base.lightTransform", GetDirectionalLightTransform(light));
		shader->UploadUniformFloat("directionalLight.base.bias", 0.005f);

		shader->UploadUniformFloat3("directionalLight.direction", light.direction);
		shader->UploadUniformFloat3("directionalLight.ambientColor", light.ambientColor);
		shader->UploadUniformFloat("directionalLight.ambientIntensity", light.ambientIntensity);

		shader->BindTexture(4, m_ShadowBuffer->GetShadowMapAttachmentID());

		if (light.shadowType != ShadowType::NONE && light.shadowMapRegion)
		{
			const auto& [topLeft, bottomRight] = *light.shadowMapRegion;
			const auto shadowMapSize = glm::vec2(
				bottomRight.x - topLeft.x,
				bottomRight.y - topLeft.y
			);

			shader->UploadUniformFloat2("directionalLight.base.shadowMapTopLeft", topLeft);
			shader->UploadUniformFloat2("directionalLight.base.shadowMapSize", shadowMapSize);
		}
	}

	void Renderer::RenderShadowMaps()
	{
		m_ShadowBuffer->Bind();
		RenderCommand::SetClearColor(m_ShadowBuffer->GetSpecs().ClearColor);
		RenderCommand::Clear();

		m_TextureAtlas.AllocateTextureAtlas(SHADOW_BUFFER_RESOLUTION);

		for (auto& light : m_DirectionalLights)
		{
			if (light.shadowType == ShadowType::NONE) continue;
			light.shadowMapRegion = m_TextureAtlas.GetTextureRegion((uint16_t) light.shadowMapResolution);
		}

		for (auto& light : m_PointLights)
		{
			if (light.shadowType == ShadowType::NONE) continue;
			light.shadowMapRegion = m_TextureAtlas.GetTextureRegion((uint16_t)light.shadowMapResolution);
		}

		for (auto& light : m_SpotLights)
		{
			if (light.shadowType == ShadowType::NONE) continue;
			light.shadowMapRegion = m_TextureAtlas.GetTextureRegion((uint16_t)light.shadowMapResolution);
		}

		if (!m_DirectionalLights.empty())
		{
			if (const DirectionalLight& light = m_DirectionalLights[0]; light.shadowType != ShadowType::NONE)
			{
				auto& [topLeft, bottomRight] = *light.shadowMapRegion;
				const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::SHADOW_MAP_DIRECTIONAL);

				m_ShadowBuffer->Bind(
					topLeft.x, topLeft.y,
					bottomRight.x - topLeft.x,
					bottomRight.y - topLeft.y);

				shader->Bind();
				shader->UploadUniformMat4("lightTransform", GetDirectionalLightTransform(light));
				shader->EnableFeature(GlFeature::POLYGON_OFFSET);
				shader->SetPolygonOffset(glm::vec2(2.0f, 4.0f));

				for (const MeshRenderCmd& cmd : m_MeshRenderCmds)
				{
					shader->UploadUniformMat4("model", cmd.transform);
					RenderMesh(cmd.vertexArray);
				}

				shader->DisableFeature(GlFeature::POLYGON_OFFSET);
				shader->Unbind();
			}
		}

		for (const auto& light : m_SpotLights)
		{
			if (light.shadowType != ShadowType::NONE)
			{
				auto& [topLeft, bottomRight] = *light.shadowMapRegion;
				const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::SHADOW_MAP_SPOT);

				m_ShadowBuffer->Bind(
					topLeft.x, topLeft.y,
					bottomRight.x - topLeft.x,
					bottomRight.y - topLeft.y);

				shader->Bind();
				shader->UploadUniformMat4("lightTransform", GetSpotLightTransform(light));
				shader->UploadUniformFloat("farPlane", light.attenuationRadius * 1.5f);
				shader->UploadUniformFloat("nearPlane", 0.1f);

				shader->EnableFeature(GlFeature::POLYGON_OFFSET);
				shader->SetPolygonOffset(glm::vec2(2.0f, 4.0f));

				for (const MeshRenderCmd& cmd : m_MeshRenderCmds)
				{
					shader->UploadUniformMat4("model", cmd.transform);
					RenderMesh(cmd.vertexArray);
				}

				shader->DisableFeature(GlFeature::POLYGON_OFFSET);
				shader->Unbind();
			}
		}
		
		m_ShadowBuffer->Unbind();
	}

	void Renderer::RenderWorld(const Ref<PerspectiveCamera>& camera, const Ref<World>& world)
	{
		SetResolution(camera->GetResolution());
		BeginScene();

		world->GetSystem<AtmosphericsSystem>()->Update(camera, m_Resolution);
		world->GetSystem<LightSystem>()->Run(camera, world);
		world->GetSystem<RenderSystem>()->Run(camera, world);
		world->GetSystem<BillboardSystem>()->Run(camera, world);

		RenderShadowMaps();

		m_RenderBuffer->Bind();
		RenderCommand::SetClearColor(m_RenderBuffer->GetSpecs().ClearColor);
		RenderCommand::Clear();

		world->GetSystem<AtmosphericsSystem>()->Run(camera);

		m_RenderBuffer->ClearAttachment(1, -1);
		PrepareLights();

		for (const auto& cmd : m_MeshRenderCmds)
			ExecuteMeshRenderCommand(camera, cmd);

		for (const auto& cmd : m_BillboardRenderCmds)
			ExecuteBillboardRenderCommand(camera, cmd);

		EndScene();
		m_RenderBuffer->Unbind();
	}

	void Renderer::ExecuteMeshRenderCommand(const Ref<PerspectiveCamera>& camera, const MeshRenderCmd& cmd)
	{
		const Ref<Shader> shader = ShaderManager::GetShaderByType(cmd.material->GetShaderType());
		shader->Bind();
		shader->SetCamera(camera->GetCameraPosition(), camera->GetViewMatrix(), camera->GetProjection());
		shader->UploadUniformMat4("model", cmd.transform);
		shader->UploadUniformInt("aEntityID", cmd.id);

		if (cmd.material) cmd.material->Bind();
		RenderMesh(cmd.vertexArray);
		shader->Unbind();
	}

	void Renderer::ExecuteBillboardRenderCommand(const Ref<PerspectiveCamera>& camera, const BillboardCmd& cmd)
	{
		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::BILLBOARD);

		shader->Bind();
		shader->SetCamera(camera->GetCameraPosition(), camera->GetViewMatrix(), camera->GetProjection());

		shader->UploadUniformMat4("model", cmd.transform);
		shader->UploadUniformInt("aEntityID", cmd.id);

		shader->EnableFeature(GlFeature::BLEND);
		shader->SetBlendMode(GlBlendOption::SRC_ALPHA, GlBlendOption::ONE_MINUS_SRC_ALPHA);
		shader->UploadUniformFloat3("TintColor", cmd.tintColor);
		cmd.texture->bind(0);

		RenderMesh(QuadMeshWorld(cmd.scale).GetSubmeshes()[0]->vertexArray);

		shader->DisableFeature(GlFeature::BLEND);
		shader->Unbind();
	}

	void Renderer::PushMeshRenderCommand(const MeshRenderCmd& cmd)
	{
		m_MeshRenderCmds.push_back(cmd);
	}

	void Renderer::PushBillboardRenderCommand(const BillboardCmd& cmd)
	{
		m_BillboardRenderCmds.push_back(cmd);
	}

	void Renderer::PushDirectionalLight(const DirectionalLight& light)
	{
		m_DirectionalLights.push_back(light);
		if (light.shadowType != ShadowType::NONE)
		{
			m_TextureAtlas.AddTexture((uint16_t)light.shadowMapResolution);
		}
	}

	void Renderer::PushPointLight(const PointLight& light)
	{
		m_PointLights.push_back(light);
		if (light.shadowType != ShadowType::NONE)
		{
			m_TextureAtlas.AddTexture((uint16_t)light.shadowMapResolution);
		}
	}

	void Renderer::PushSpotLight(const SpotLight& light)
	{
		m_SpotLights.push_back(light);
		if (light.shadowType != ShadowType::NONE)
		{
			m_TextureAtlas.AddTexture((uint16_t)light.shadowMapResolution);
		}
	}

	void Renderer::BeginScene()
	{
		m_TextureAtlas.Clear();

		m_DirectionalLights.clear();
		m_PointLights.clear();
		m_SpotLights.clear();

		m_MeshRenderCmds.clear();
		m_BillboardRenderCmds.clear();
	}

	void Renderer::EndScene(){}

	void Renderer::InitShadowBuffer()
	{
		FramebufferSpecs specs;
		specs.Width = SHADOW_BUFFER_RESOLUTION.x;
		specs.Height = SHADOW_BUFFER_RESOLUTION.y;

		specs.HasShadowMapAttachment = true;
		specs.ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		specs.Attachments = {
			FramebufferTextureFormat::RGBA8
		};

		m_ShadowBuffer = FramebufferManager::CreateFramebuffer("ShadowBuffer", specs);
	}

	void Renderer::RenderMesh(const Ref<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
		vertexArray->Unbind();
	}

	glm::mat4 Renderer::GetDirectionalLightProjection(const DirectionalLight& light)
	{
		return glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -25.0f, 25.0f);
	}

	glm::mat4 Renderer::GetPointLightProjection(const PointLight& light)
	{
		return glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -25.0f, 25.0f);
	}

	glm::mat4 Renderer::GetSpotLightProjection(const SpotLight& light) {
		return glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, light.attenuationRadius * 1.5f);
	}

	glm::mat4 Renderer::GetDirectionalLightTransform(const DirectionalLight& light)
	{
		return GetDirectionalLightProjection(light) * lookAt(light.direction, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::mat4 Renderer::GetPointLightTransform(const PointLight& light)
	{
		return glm::mat4(1.0f);
	}

	glm::mat4 Renderer::GetSpotLightTransform(const SpotLight& light)
	{
		return GetSpotLightProjection(light) * lookAt(light.position, light.position - light.direction, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	void Renderer::SetPointLightCount(const Ref<Shader>& shader, const int lightCount)
	{
		shader->UploadUniformInt("pointLightCount", lightCount);
	}

	void Renderer::SetSpotLightCount(const Ref<Shader>& shader, const int lightCount)
	{
		shader->UploadUniformInt("spotLightCount", lightCount);
	}

	void Renderer::AddSpotLight(const size_t index, const Ref<Shader>& shader, const SpotLight& spotLight, const glm::mat4& lightTransform)
	{
		const std::string base = "spotLights[" + std::to_string(index) + "]";

		shader->UploadUniformFloat3(base + ".base.base.color", spotLight.color);
		shader->UploadUniformFloat(base + ".base.base.intensity", spotLight.intensity);
		shader->UploadUniformFloat(base + ".base.base.isShadowCasting", spotLight.shadowType != ShadowType::NONE);
		shader->UploadUniformFloat(base + ".base.base.useSoftShadow", spotLight.shadowType == ShadowType::SOFT);
		shader->UploadUniformMat4(base + ".base.base.lightTransform", lightTransform);
		shader->UploadUniformFloat(base + ".base.base.bias", 0.00005f);

		shader->UploadUniformFloat3(base + ".base.position", spotLight.position);
		shader->UploadUniformFloat(base + ".base.attenuationRadius", spotLight.attenuationRadius);

		shader->UploadUniformFloat3(base + ".direction", spotLight.direction);
		shader->UploadUniformFloat(base + ".attenuationAngle", spotLight.attenuationAngle);

		if (spotLight.shadowType != ShadowType::NONE && spotLight.shadowMapRegion)
		{
			const auto& [topLeft, bottomRight] = *spotLight.shadowMapRegion;
			const auto shadowMapSize = glm::vec2(
				bottomRight.x - topLeft.x,
				bottomRight.y - topLeft.y
			);

			shader->UploadUniformFloat2(base + ".base.base.shadowMapTopLeft", topLeft);
			shader->UploadUniformFloat2(base + ".base.base.shadowMapSize", shadowMapSize);
		}

	}



	void Renderer::AddPointLight(const size_t index, const Ref<Shader>& shader, const PointLight& pointLight, const glm::mat4& lightTransform)
	{
		const std::string base = "pointLights[" + std::to_string(index) + "]";

		shader->UploadUniformFloat3(base + ".base.color", pointLight.color);
		shader->UploadUniformFloat(base + ".base.intensity", pointLight.intensity);
		shader->UploadUniformMat4(base + ".base.lightTransform", lightTransform);

		shader->UploadUniformFloat3(base + ".position", pointLight.position);
		shader->UploadUniformFloat(base + ".attenuationRadius", pointLight.attenuationRadius);
	}
};
