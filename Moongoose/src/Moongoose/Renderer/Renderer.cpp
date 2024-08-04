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

namespace Moongoose {

	glm::uvec2 Renderer::m_Resolution;
	Ref<Framebuffer> Renderer::m_RenderBuffer;
	Ref<Framebuffer> Renderer::m_ShadowBuffer;

	std::vector<Renderer::DirectionalLight> Renderer::m_DirectionalLights;
	std::vector<Renderer::PointLight> Renderer::m_PointLights;
	std::vector<Renderer::SpotLight> Renderer::m_SpotLights;

	std::vector<Renderer::MeshRenderCmd> Renderer::m_MeshRenderCmds;
	std::vector<Renderer::BillboardCmd> Renderer::m_BillboardRenderCmds;

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
		shader->ResetLights();

		if (!m_DirectionalLights.empty())
		{
			const DirectionalLight light = m_DirectionalLights[0];
			shader->UploadUniformMat4("lightTransform", GetDirectionalLightProjection(light) * lookAt(light.direction, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
			shader->SetDirectionalLight(light.direction, light.color, light.ambientColor, light.intensity, light.ambientIntensity, light.isShadowCasting, true);
			shader->BindTexture(4, m_ShadowBuffer->GetShadowMapAttachmentID());
		}

		for (const auto& pointLight : m_PointLights)
		{
			shader->SetPointLight(
				pointLight.position,
				pointLight.color,
				pointLight.intensity,
				pointLight.attenuationRadius);
		}

		for (const auto& spotLight : m_SpotLights)
		{
			shader->SetSpotLight(
				spotLight.direction,
				spotLight.position,
				spotLight.color,
				spotLight.intensity,
				spotLight.attenuationRadius,
				spotLight.attenuationAngle);
		}

		shader->Unbind();
	}

	void Renderer::RenderShadowMaps()
	{
		if (!m_DirectionalLights.empty())
		{
			const DirectionalLight& light = m_DirectionalLights[0];
			const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::SHADOW_MAP);
			const glm::mat4 projection = GetDirectionalLightProjection(light);

			m_ShadowBuffer->Bind();

			RenderCommand::SetClearColor(m_ShadowBuffer->GetSpecs().ClearColor);
			RenderCommand::Clear();

			shader->Bind();
			shader->UploadUniformMat4("lightTransform", projection * glm::lookAt(light.direction, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
			shader->EnablePolygonOffset(true);
			shader->SetPolygonOffset(glm::vec2(2.0f, 4.0f));

			for (const MeshRenderCmd& cmd : m_MeshRenderCmds)
			{
				shader->SetModelTransform(cmd.transform);
				RenderMesh(cmd.vertexArray);
			}

			shader->EnablePolygonOffset(false);
			shader->Unbind();
			m_ShadowBuffer->Unbind();
		}
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
		shader->SetModelTransform(cmd.transform);
		shader->SetEntityId(cmd.id);

		if (cmd.material) cmd.material->Bind();
		RenderMesh(cmd.vertexArray);
		shader->Unbind();
	}

	void Renderer::ExecuteBillboardRenderCommand(const Ref<PerspectiveCamera>& camera, const BillboardCmd& cmd)
	{
		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::BILLBOARD);

		shader->Bind();
		shader->SetCamera(camera->GetCameraPosition(), camera->GetViewMatrix(), camera->GetProjection());
		shader->SetModelTransform(cmd.transform);
		shader->SetEntityId(cmd.id);
		shader->SetBlendMode(true);
		shader->UploadUniformFloat3("TintColor", cmd.tintColor);
		cmd.texture->bind(0);

		RenderMesh(QuadMeshWorld(cmd.scale).GetSubmeshes()[0]->vertexArray);

		shader->SetBlendMode(false);
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

	void Renderer::PushDirectionalLight(const DirectionalLight& directionalLight)
	{
		m_DirectionalLights.push_back(directionalLight);
	}

	void Renderer::PushPointLight(const PointLight& pointLight)
	{
		m_PointLights.push_back(pointLight);
	}

	void Renderer::PushSpotLight(const SpotLight& spotLight)
	{
		m_SpotLights.push_back(spotLight);
	}

	void Renderer::BeginScene()
	{
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
		//specs.Width = 4096;
		//specs.Height = 4096;

		specs.Width = 2048;
		specs.Height = 2048;

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
		return glm::perspective(light.attenuationAngle, (float) 4096 / 4096, 0.1f, 1000.0f);
	}
};
