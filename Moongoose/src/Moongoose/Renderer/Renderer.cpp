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

namespace Moongoose
{
	glm::uvec2 Renderer::m_Resolution;

	Ref<GBuffer> Renderer::m_GBuffer;
	Ref<Framebuffer> Renderer::m_PostProcessingBuffer;
	Ref<Framebuffer> Renderer::m_RenderBuffer;
	Ref<Framebuffer> Renderer::m_ShadowBuffer;
	Ref<Framebuffer> Renderer::m_PointShadowBuffer;

	TextureAtlas Renderer::m_TextureAtlas;

	std::vector<DirectionalLight> Renderer::m_DirectionalLights;
	std::vector<PointLight> Renderer::m_PointLights;
	std::vector<SpotLight> Renderer::m_SpotLights;

	std::vector<MeshCommand> Renderer::m_MeshRenderCmds;
	std::vector<BillboardCommand> Renderer::m_BillboardRenderCmds;

	unsigned int Renderer::prevDrawCount = 0;
	unsigned int Renderer::currentDrawCount = 0;

	void Renderer::InitShadowBuffer()
	{
		FramebufferSpecs specs;
		specs.width = SHADOW_BUFFER_RESOLUTION.x;
		specs.height = SHADOW_BUFFER_RESOLUTION.y;

		specs.hasShadowMapAttachment = true;
		specs.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		specs.attachments = {
			FramebufferTextureFormat::RGBA8
		};

		m_ShadowBuffer = FramebufferManager::CreateFramebuffer("ShadowBuffer", specs);

		FramebufferSpecs pointShadowSpecs;
		pointShadowSpecs.width = CUBE_SHADOW_MAP_RESOLUTION;
		pointShadowSpecs.height = CUBE_SHADOW_MAP_RESOLUTION;

		pointShadowSpecs.hasShadowMapCubeAttachment = true;
		pointShadowSpecs.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

		m_PointShadowBuffer = FramebufferManager::CreateFramebuffer("PointShadowBuffer", pointShadowSpecs);
	}

	void Renderer::InitGBuffer()
	{
		m_GBuffer = CreateRef<GBuffer>(GBuffer::GBufferSpecs({m_Resolution.x, m_Resolution.y}));
	}

	void Renderer::InitRenderBuffer()
	{
		FramebufferSpecs specs;
		specs.width = m_Resolution.x;
		specs.height = m_Resolution.y;
		specs.attachments = {
			FramebufferTextureFormat::RGBA8,
			FramebufferTextureFormat::RED_INTEGER,
			FramebufferTextureFormat::DEPTH24STENCIL8
		};
		specs.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

		m_RenderBuffer = FramebufferManager::CreateFramebuffer("RenderBuffer", specs);
	}

	void Renderer::InitPostProcessingBuffer()
	{
		FramebufferSpecs specs;
		specs.width = m_Resolution.x;
		specs.height = m_Resolution.y;
		specs.attachments = {
			FramebufferTextureFormat::RGBA8
		};
		specs.clearColor = {0.0f, 0.0f, 0.0f, 1.0f};

		m_PostProcessingBuffer = FramebufferManager::CreateFramebuffer("PostProcessingBuffer", specs);
	}

	void Renderer::SetResolution(const glm::uvec2 newResolution)
	{
		if (newResolution == m_Resolution) return;
		m_Resolution = newResolution;

		// Init framebuffers
		if (!m_ShadowBuffer || !m_PointShadowBuffer) InitShadowBuffer();
		if (!m_GBuffer) InitGBuffer();
		if (!m_PostProcessingBuffer) InitPostProcessingBuffer();
		if (!m_RenderBuffer) InitRenderBuffer();
	}

	void Renderer::PrepareLights()
	{
		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::STATIC);
		shader->Bind();

		shader->SetFloat("directionalLight.base.intensity", 0.0f);
		shader->SetFloat3("directionalLight.base.color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		shader->SetFloat3("directionalLight.direction", glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));

		shader->BindTexture(4, m_ShadowBuffer->GetShadowMapAttachmentID());
		shader->BindCubeMapTexture(5, m_PointShadowBuffer->GetShadowMapCubeAttachmentID());

		for (const auto& light : m_DirectionalLights) AddDirectionalLight(light, shader);

		shader->UploadUniformInt("pointLightCount", m_PointLights.size());
		for (size_t i = 0; i < m_PointLights.size(); i++) AddPointLight(i, shader, m_PointLights[i]);

		shader->UploadUniformInt("spotLightCount", m_SpotLights.size());
		for (size_t i = 0; i < m_SpotLights.size(); i++) AddSpotLight(i, shader, m_SpotLights[i]);


		shader->Unbind();
	}

	void Renderer::AddDirectionalLight(const DirectionalLight& light, const Ref<Shader>& shader)
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

		shader->BindTexture(4, m_ShadowBuffer->GetShadowMapAttachmentID());

		if (light.shadowType != ShadowType::NONE && light.shadowMapRegion)
		{
			const auto& [topLeft, bottomRight] = *light.shadowMapRegion;
			const auto shadowMapSize = glm::vec2(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);

			shader->SetFloat2("directionalLight.base.shadowMapTopLeft", topLeft);
			shader->SetFloat2("directionalLight.base.shadowMapSize", shadowMapSize);
		}
	}

	Ref<Framebuffer> Renderer::GetRenderBuffer()
	{
		return m_PostProcessingBuffer;
	}

	void Renderer::RenderShadowMaps()
	{
		m_ShadowBuffer->Bind();
		RenderCommand::SetClearColor(m_ShadowBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		m_TextureAtlas.AllocateTextureAtlas(SHADOW_BUFFER_RESOLUTION);

		for (auto& light : m_DirectionalLights)
		{
			if (light.shadowType == ShadowType::NONE) continue;
			light.shadowMapRegion = m_TextureAtlas.GetTextureRegion((uint16_t)light.shadowMapResolution);

			auto& [topLeft, bottomRight] = *light.shadowMapRegion;
			const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::SHADOW_MAP_DIRECTIONAL);

			m_ShadowBuffer->Bind(
				topLeft.x, topLeft.y,
				bottomRight.x - topLeft.x,
				bottomRight.y - topLeft.y);

			shader->Bind();
			shader->SetMat4("light.lightTransform", light.GetTransform());

			shader->EnableFeature(GlFeature::POLYGON_OFFSET);
			shader->SetPolygonOffset(glm::vec2(2.0f, 4.0f));

			for (const MeshCommand& cmd : m_MeshRenderCmds)
			{
				shader->SetMat4("model", cmd.transform);
				RenderMesh(cmd.vertexArray);
			}

			shader->DisableFeature(GlFeature::POLYGON_OFFSET);
			shader->Unbind();
		}

		for (auto& light : m_SpotLights)
		{
			if (light.shadowType == ShadowType::NONE) continue;
			light.shadowMapRegion = m_TextureAtlas.GetTextureRegion(static_cast<uint16_t>(light.shadowMapResolution));

			auto& [topLeft, bottomRight] = *light.shadowMapRegion;
			const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::SHADOW_MAP_SPOT);

			m_ShadowBuffer->Bind(
				topLeft.x, topLeft.y,
				bottomRight.x - topLeft.x,
				bottomRight.y - topLeft.y);

			shader->Bind();
			shader->SetMat4("light.lightTransform", light.GetTransform());
			shader->SetFloat("farPlane", light.attenuationRadius * 1.5f);
			shader->SetFloat("nearPlane", 0.1f);

			shader->EnableFeature(GlFeature::POLYGON_OFFSET);
			shader->SetPolygonOffset(glm::vec2(2.0f, 4.0f));

			for (const MeshCommand& cmd : m_MeshRenderCmds)
			{
				shader->SetMat4("model", cmd.transform);
				RenderMesh(cmd.vertexArray);
			}

			shader->DisableFeature(GlFeature::POLYGON_OFFSET);
			shader->Unbind();
		}

		m_ShadowBuffer->Unbind();

		m_PointShadowBuffer->Bind();
		RenderCommand::SetClearColor(m_PointShadowBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		for (auto& light : m_PointLights)
		{
			if (light.shadowType == ShadowType::NONE) continue;
			auto shadowMatrices = light.GetTransform();

			const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::SHADOW_MAP_POINT);
			shader->Bind();

			shader->SetMat4("shadowMatrices[0]", shadowMatrices[0]);
			shader->SetMat4("shadowMatrices[1]", shadowMatrices[1]);
			shader->SetMat4("shadowMatrices[2]", shadowMatrices[2]);
			shader->SetMat4("shadowMatrices[3]", shadowMatrices[3]);
			shader->SetMat4("shadowMatrices[4]", shadowMatrices[4]);
			shader->SetMat4("shadowMatrices[5]", shadowMatrices[5]);

			shader->SetFloat3("lightPos", light.position);

			shader->SetFloat("farPlane", light.attenuationRadius * 1.5f);
			shader->SetFloat("nearPlane", 0.1f);

			shader->EnableFeature(GlFeature::POLYGON_OFFSET);
			shader->SetPolygonOffset(glm::vec2(2.0f, 4.0f));

			for (const MeshCommand& cmd : m_MeshRenderCmds)
			{
				shader->SetMat4("model", cmd.transform);
				RenderMesh(cmd.vertexArray);
			}

			shader->DisableFeature(GlFeature::POLYGON_OFFSET);
			shader->Unbind();
		}
		m_PointShadowBuffer->Unbind();
	}

	void Renderer::RenderGBuffer(const Ref<PerspectiveCamera>& camera)
	{
		m_GBuffer->Bind();
		RenderCommand::SetClearColor(m_GBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::GBUFFER);
		shader->Bind();

		for (const MeshCommand& cmd : m_MeshRenderCmds)
		{
			shader->SetCamera(camera->GetCameraPosition(), camera->GetViewMatrix(), camera->GetProjection());

			if (cmd.material->m_Normal) cmd.material->m_Normal->Bind(0);
			if (cmd.material->m_Roughness) cmd.material->m_Roughness->Bind(1);

			shader->UploadUniformInt("bUseNormalMap", cmd.material->m_Normal != nullptr ? 1 : 0);
			shader->UploadUniformInt("bUseRoughnessMap", cmd.material->m_Roughness != nullptr ? 1 : 0);

			shader->SetFloat("roughnessValue", cmd.material->m_RoughnessValue);

			shader->SetMat4("model", cmd.transform);
			RenderMesh(cmd.vertexArray);
		}

		shader->Unbind();
		m_GBuffer->Unbind();
	}

	void Renderer::RenderPostProcessing(const Ref<PerspectiveCamera>& camera)
	{
		m_PostProcessingBuffer->Bind();
		RenderCommand::SetClearColor(m_PostProcessingBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::POST_PROCESS_SSR);
		shader->Bind();
		shader->SetCamera(camera->GetCameraPosition(), camera->GetViewMatrix(), camera->GetProjection());
		shader->BindTexture(0, m_RenderBuffer->GetColorAttachments()[0]);
		shader->BindTexture(1, m_GBuffer->GetViewPositionTexutre());
		shader->BindTexture(2, m_GBuffer->GetNormalTexture());
		shader->BindTexture(3, m_GBuffer->GetRoughnessTexture());
		shader->BindTexture(4, m_GBuffer->GetGBuffer()->GetDepthAttachment());

		RenderCommand::DrawIndexed(QuadMesh().GetSubmeshes()[0]->vertexArray);

		shader->Unbind();
		m_PostProcessingBuffer->Unbind();
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
		RenderGBuffer(camera);

		m_RenderBuffer->Bind();
		RenderCommand::SetClearColor(m_RenderBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		world->GetSystem<AtmosphericsSystem>()->Run(camera);

		m_RenderBuffer->ClearAttachment(1, -1);
		PrepareLights();

		for (const auto& cmd : m_MeshRenderCmds)
			ExecuteMeshRenderCommand(camera, cmd);

		for (const auto& cmd : m_BillboardRenderCmds)
			ExecuteBillboardRenderCommand(camera, cmd);

		m_RenderBuffer->Unbind();


		RenderPostProcessing(camera);

		EndScene();
	}

	void Renderer::ExecuteMeshRenderCommand(const Ref<PerspectiveCamera>& camera, const MeshCommand& cmd)
	{
		const Ref<Shader> shader = ShaderManager::GetShaderByType(cmd.material->GetShaderType());
		shader->Bind();

		shader->SetCamera(camera->GetCameraPosition(), camera->GetViewMatrix(), camera->GetProjection());
		shader->SetMat4("model", cmd.transform);
		shader->UploadUniformInt("aEntityID", cmd.id);

		shader->UploadUniformInt("material.useAlbedoTexture", cmd.material->m_Albedo != nullptr ? 1 : 0);
		shader->UploadUniformInt("material.useMetallicTexture", cmd.material->m_Metallic != nullptr ? 1 : 0);
		shader->UploadUniformInt("material.useRoughnessTexture", cmd.material->m_Roughness != nullptr ? 1 : 0);
		shader->UploadUniformInt("material.useNormalMapTexture", cmd.material->m_Normal != nullptr ? 1 : 0);

		shader->SetFloat3("material.albedo", cmd.material->m_AlbedoColor);
		shader->SetFloat("material.roughness", cmd.material->m_RoughnessValue);
		shader->SetFloat("material.metallic", cmd.material->m_MetallicValue);

		if (cmd.material) cmd.material->Bind();
		RenderMesh(cmd.vertexArray);
		shader->Unbind();
	}

	void Renderer::ExecuteBillboardRenderCommand(const Ref<PerspectiveCamera>& camera, const BillboardCommand& cmd)
	{
		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::BILLBOARD);

		shader->Bind();
		shader->EnableFeature(GlFeature::BLEND);
		shader->SetBlendMode(GlBlendOption::SRC_ALPHA, GlBlendOption::ONE_MINUS_SRC_ALPHA);

		shader->SetCamera(camera->GetCameraPosition(), camera->GetViewMatrix(), camera->GetProjection());

		shader->SetMat4("model", cmd.transform);
		shader->UploadUniformInt("aEntityID", cmd.id);

		shader->SetFloat3("TintColor", cmd.tintColor);
		cmd.texture->Bind(0);

		RenderMesh(QuadMeshWorld(cmd.scale).GetSubmeshes()[0]->vertexArray);

		shader->DisableFeature(GlFeature::BLEND);
		shader->Unbind();
	}

	void Renderer::PushMeshRenderCommand(const MeshCommand& cmd)
	{
		m_MeshRenderCmds.push_back(cmd);
	}

	void Renderer::PushBillboardRenderCommand(const BillboardCommand& cmd)
	{
		m_BillboardRenderCmds.push_back(cmd);
	}

	void Renderer::PushDirectionalLight(const DirectionalLight& light)
	{
		m_DirectionalLights.push_back(light);
		if (light.shadowType > ShadowType::NONE) m_TextureAtlas.AddTexture((uint16_t)light.shadowMapResolution);
	}

	void Renderer::PushPointLight(const PointLight& light)
	{
		m_PointLights.push_back(light);
	}

	void Renderer::PushSpotLight(const SpotLight& light)
	{
		m_SpotLights.push_back(light);
		if (light.shadowType > ShadowType::NONE) m_TextureAtlas.AddTexture((uint16_t)light.shadowMapResolution);
	}

	void Renderer::BeginScene()
	{
		prevDrawCount = currentDrawCount;
		currentDrawCount = 0;

		m_TextureAtlas.Clear();

		m_DirectionalLights.clear();
		m_PointLights.clear();
		m_SpotLights.clear();

		m_MeshRenderCmds.clear();
		m_BillboardRenderCmds.clear();
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::RenderMesh(const Ref<VertexArray>& vertexArray)
	{
		RenderCommand::DrawIndexed(vertexArray);
		currentDrawCount++;
	}

	void Renderer::AddSpotLight(const size_t index, const Ref<Shader>& shader, const SpotLight& spotLight)
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

	void Renderer::AddPointLight(const size_t index, const Ref<Shader>& shader, const PointLight& pointLight)
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
