#include "mgpch.h"
#include "RenderPass.h"
#include "Moongoose/Renderer/FramebufferManager.h"
#include "Moongoose/Renderer/ShaderManager.h"

namespace Moongoose
{
	void ShadowMapPass::Render(Ref<Framebuffer> targetBuffer, RenderPassParams& renderPassParams)
	{
		const auto data = static_cast<ShadowMapPassData*>(renderPassParams.additionalData);

		if (!m_ShadowBuffer || !m_PointShadowBuffer) InitFramebuffer();
		m_TextureAtlas.Clear();

		for (auto& light : data->directionalLights)
			if (light.shadowType > ShadowType::NONE)
				m_TextureAtlas.AddTexture((uint16_t)light.shadowMapResolution);


		for (auto& light : data->spotLights)
			if (light.shadowType > ShadowType::NONE)
				m_TextureAtlas.AddTexture((uint16_t)light.shadowMapResolution);


		for (auto& light : data->pointLights)
			if (light.shadowType > ShadowType::NONE)
				m_TextureAtlas.AddTexture((uint16_t)light.shadowMapResolution);


		m_ShadowBuffer->Bind();
		RenderCommand::SetClearColor(m_ShadowBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		m_TextureAtlas.AllocateTextureAtlas(SHADOW_BUFFER_RESOLUTION);

		for (auto& light : data->directionalLights)
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

			for (const MeshCommand& cmd : renderPassParams.meshCommands)
			{
				shader->SetMat4("model", cmd.transform);
				RenderCommand::DrawIndexed(cmd.vertexArray);
			}

			shader->DisableFeature(GlFeature::POLYGON_OFFSET);
			shader->Unbind();
		}

		for (auto& light : data->spotLights)
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

			for (const MeshCommand& cmd : renderPassParams.meshCommands)
			{
				shader->SetMat4("model", cmd.transform);
				RenderCommand::DrawIndexed(cmd.vertexArray);
			}

			shader->DisableFeature(GlFeature::POLYGON_OFFSET);
			shader->Unbind();
		}

		m_ShadowBuffer->Unbind();

		m_PointShadowBuffer->Bind();
		RenderCommand::SetClearColor(m_PointShadowBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		for (auto& light : data->pointLights)
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

			for (const MeshCommand& cmd : renderPassParams.meshCommands)
			{
				shader->SetMat4("model", cmd.transform);
				RenderCommand::DrawIndexed(cmd.vertexArray);
			}

			shader->DisableFeature(GlFeature::POLYGON_OFFSET);
			shader->Unbind();
		}
		m_PointShadowBuffer->Unbind();
	}

	void ShadowMapPass::InitFramebuffer()
	{
		FramebufferSpecs specs;
		specs.width = SHADOW_BUFFER_RESOLUTION.x;
		specs.height = SHADOW_BUFFER_RESOLUTION.y;

		specs.hasShadowMapAttachment = true;
		specs.attachments = {FramebufferTextureFormat::RGBA8};

		m_ShadowBuffer = FramebufferManager::CreateFramebuffer("ShadowBuffer");
		m_ShadowBuffer->Configure(specs);

		FramebufferSpecs pointShadowSpecs;
		pointShadowSpecs.width = CUBE_SHADOW_MAP_RESOLUTION;
		pointShadowSpecs.height = CUBE_SHADOW_MAP_RESOLUTION;
		pointShadowSpecs.hasShadowMapCubeAttachment = true;

		m_PointShadowBuffer = FramebufferManager::CreateFramebuffer("PointShadowBuffer");
		m_PointShadowBuffer->Configure(pointShadowSpecs);
	}
}
