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

	std::vector<Renderer::AtlasBox>				Renderer::m_ShadowMapAtlas;

	std::vector<Renderer::DirectionalLight>		Renderer::m_DirectionalLights;
	std::vector<Renderer::PointLight>			Renderer::m_PointLights;
	std::vector<Renderer::SpotLight>			Renderer::m_SpotLights;

	std::vector<Renderer::DirectionalLight>		Renderer::m_ShadowCastingDirectionalLights;
	std::vector<Renderer::PointLight>			Renderer::m_ShadowCastingPointLights;
	std::vector<Renderer::SpotLight>			Renderer::m_ShadowCastingSpotLights;

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
		shader->ResetLights();

		for (const auto& light : m_DirectionalLights) PrepareDirectionalLight(light, shader);
		for (const auto& light : m_PointLights) PreparePointLight(light, shader);
		for (const auto& light : m_SpotLights) PrepareSpotLight(light, shader);

		shader->Unbind();
	}

	void Renderer::PrepareDirectionalLight(const DirectionalLight& light, const Ref<Shader>& shader)
	{
		shader->UploadUniformMat4("lightTransform", GetDirectionalLightProjection(light) * lookAt(light.direction, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f)));
		shader->SetDirectionalLight(light.direction, light.color, light.ambientColor, light.intensity, light.ambientIntensity, light.isShadowCasting, true);
		shader->BindTexture(4, m_ShadowBuffer->GetShadowMapAttachmentID());

		if (light.isShadowCasting && m_ShadowMapAtlas.size() > 0)
		{
			const AtlasBox& shadowMapAtlas = m_ShadowMapAtlas[0];
			const auto topLeft = glm::vec2(shadowMapAtlas.topLeft.x, shadowMapAtlas.topLeft.y);
			const auto shadowMapSize = glm::vec2(
				shadowMapAtlas.bottomRight.x - shadowMapAtlas.topLeft.x,
				shadowMapAtlas.bottomRight.y - shadowMapAtlas.topLeft.y
			);

			shader->UploadUniformFloat2("directionalLight.base.shadowMapTopLeft", topLeft);
			shader->UploadUniformFloat2("directionalLight.base.shadowMapSize", shadowMapSize);
		}
	}

	void Renderer::PreparePointLight(const PointLight& light, const Ref<Shader>& shader)
	{
		shader->SetPointLight(
			light.position,
			light.color,
			light.intensity,
			light.attenuationRadius);
	}

	void Renderer::PrepareSpotLight(const SpotLight& light, const Ref<Shader>& shader)
	{
		shader->SetSpotLight(
			light.direction,
			light.position,
			light.color,
			light.intensity,
			light.attenuationRadius,
			light.attenuationAngle);
	}

	void Renderer::RenderShadowMaps()
	{
		m_ShadowBuffer->Bind();
		RenderCommand::SetClearColor(m_ShadowBuffer->GetSpecs().ClearColor);
		RenderCommand::Clear();

		std::vector<uint16_t> shadowMapSizes;
		for (const auto& light : m_ShadowCastingDirectionalLights)
			shadowMapSizes.push_back(static_cast<uint16_t>(light.shadowMapResolution));

		for (const auto& light : m_ShadowCastingPointLights)
			shadowMapSizes.push_back(static_cast<uint16_t>(light.shadowMapResolution));

		for (const auto& light : m_ShadowCastingSpotLights)
			shadowMapSizes.push_back(static_cast<uint16_t>(light.shadowMapResolution));

		if (shadowMapSizes.empty()) return;

		m_ShadowMapAtlas = AllocateTextureAtlas(SHADOW_BUFFER_RESOLUTION, shadowMapSizes);
		if (!m_DirectionalLights.empty())
		{
			auto& [topLeft, bottomRight] = m_ShadowMapAtlas[0];

			const DirectionalLight& light = m_DirectionalLights[0];
			const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::SHADOW_MAP);
			const glm::mat4 projection = GetDirectionalLightProjection(light);

			m_ShadowBuffer->Bind(
				topLeft.x, topLeft.y,
				bottomRight.x - topLeft.x,
				bottomRight.y - topLeft.y);

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
		{
			ExecuteMeshRenderCommand(camera, cmd);
		}

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
		if (directionalLight.isShadowCasting) m_ShadowCastingDirectionalLights.push_back(directionalLight);
	}

	void Renderer::PushPointLight(const PointLight& pointLight)
	{
		m_PointLights.push_back(pointLight);
		if (pointLight.isShadowCasting) m_ShadowCastingPointLights.push_back(pointLight);
	}

	void Renderer::PushSpotLight(const SpotLight& spotLight)
	{
		m_SpotLights.push_back(spotLight);
		if (spotLight.isShadowCasting) m_ShadowCastingSpotLights.push_back(spotLight);
	}

	void Renderer::BeginScene()
	{
		m_ShadowMapAtlas.clear();

		m_DirectionalLights.clear();
		m_PointLights.clear();
		m_SpotLights.clear();

		m_ShadowCastingDirectionalLights.clear();
		m_ShadowCastingPointLights.clear();
		m_ShadowCastingSpotLights.clear();

		m_MeshRenderCmds.clear();
		m_BillboardRenderCmds.clear();
	}

	void Renderer::EndScene(){}

	void Renderer::InitShadowBuffer()
	{
		FramebufferSpecs specs;
		specs.Width = 8196;
		specs.Height = 8196;

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

	/*
	 *	Credit:
	 *	https://lisyarus.github.io/blog/posts/texture-packing.html
	 */
	std::vector<Renderer::AtlasBox> Renderer::AllocateTextureAtlas(glm::uvec2 const& atlasSize, std::vector<uint16_t> const& textureSizes)
	{
		// we have to separately sort the indices so that the i-th region
		// of the output corresponds to the i-th texture size of the input
		std::vector<uint16_t> sorted(textureSizes.size());
		for (uint16_t i = 0; i < sorted.size(); ++i)
			sorted[i] = i;

		// sort in descending order
		std::sort(sorted.begin(), sorted.end(), [&](const unsigned int i, const unsigned int j) {
			return textureSizes[i] > textureSizes[j];
		});

		glm::uvec2 pen{ 0, 0 };
		std::vector<glm::uvec2> ladder;
		std::vector<AtlasBox> result(textureSizes.size());

		for (const unsigned int i : sorted)
		{
			int const size = textureSizes[i];

			// allocate a texture region
			result[i] = { {pen.x, pen.y}, {pen.x + size, pen.y + size} };

			// shift the pen to the right
			pen.x += size;

			// update the ladder
			if (!ladder.empty() && ladder.back().y == pen.y + size)
				ladder.back().x = pen.x;
			else
				ladder.emplace_back(pen.x, pen.y + size);

			if (pen.x == atlasSize.x)
			{
				// the pen hit the right edge of the atlas
				ladder.pop_back();

				pen.x = !ladder.empty() ? ladder.back().x : 0;
				pen.y += size;
			}
		}

		return result;
	}
};
