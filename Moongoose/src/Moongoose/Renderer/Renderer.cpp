#include "mgpch.h"
#include "Renderer.h"

#include "FramebufferManager.h"
#include "Light.h"
#include "MeshPrimitives.h"
#include "RenderCommand.h"
#include "Moongoose/ECS/World.h"
#include "Moongoose/Renderer/Framebuffer.h"
#include "Moongoose/ECS/Components.h"

namespace Moongoose
{
	glm::uvec2 Renderer::m_Resolution;

	Ref<Framebuffer> Renderer::m_RenderBuffer;

	SsrPass Renderer::m_SsrPass;
	LightingPass Renderer::m_LightingPass;
	GeometryPass Renderer::m_GeometryPass;
	ShadowMapPass Renderer::m_ShadowMapPass;
	BillboardPass Renderer::m_BillboardPass;
	BoxBlurPass Renderer::m_BoxBlurPass;
	SkyPass Renderer::m_SkyPass;

	std::vector<DirectionalLight> Renderer::m_DirectionalLights;
	std::vector<PointLight> Renderer::m_PointLights;
	std::vector<SpotLight> Renderer::m_SpotLights;

	std::vector<MeshCommand> Renderer::m_MeshRenderCmds;
	std::vector<BillboardCommand> Renderer::m_BillboardRenderCmds;

	unsigned int Renderer::prevDrawCount = 0;

	namespace Utils
	{
		static PointLight LightComponentToPointLight(const TransformComponent& cTransform, const LightComponent& cLight)
		{
			return {
				cLight.m_Color,
				cLight.m_Intensity,
				LightComponent::GetDefaultShadowBias(cLight.m_Type),
				cLight.m_ShadowType,
				cLight.m_ShadowMapResolution,
				nullptr,
				cTransform.m_Position,
				cLight.m_AttenuationRadius
			};
		}

		static DirectionalLight LightComponentToDirectionalLight(const TransformComponent& cTransform,
		                                                         const LightComponent& cLight)
		{
			return {
				cLight.m_Color,
				cLight.m_Intensity,
				LightComponent::GetDefaultShadowBias(cLight.m_Type),
				cLight.m_ShadowType,
				cLight.m_ShadowMapResolution,
				nullptr,
				cTransform.GetForwardDirection(),
				cLight.m_Color,
				cLight.m_Intensity * cLight.m_AmbientIntensity
			};
		}

		static SpotLight LightComponentToSpotLight(const TransformComponent& cTransform, const LightComponent& cLight)
		{
			return {
				cLight.m_Color,
				cLight.m_Intensity,
				LightComponent::GetDefaultShadowBias(cLight.m_Type),
				cLight.m_ShadowType,
				cLight.m_ShadowMapResolution,
				nullptr,
				cTransform.m_Position,
				cLight.m_AttenuationRadius,
				cTransform.GetForwardDirection(),
				cLight.m_AttenuationAngle
			};
		}
	}

	void Renderer::BeginScene()
	{
		prevDrawCount = 0;
		RenderCommand::ResetCounter();

		m_DirectionalLights.clear();
		m_PointLights.clear();
		m_SpotLights.clear();

		m_MeshRenderCmds.clear();
		m_BillboardRenderCmds.clear();
	}

	int Renderer::ReadEntityId(const uint32_t x, const uint32_t y)
	{
		m_RenderBuffer->Bind();
		const int entityId = m_RenderBuffer->ReadPixel(1, x, y);
		m_RenderBuffer->Unbind();
		return entityId;
	}

	void Renderer::SetResolution(const glm::uvec2 newResolution)
	{
		if (newResolution == m_Resolution) return;
		m_Resolution = newResolution;

		m_GeometryPass.Resize(m_Resolution);
		m_LightingPass.Resize(m_Resolution);
		m_SsrPass.Resize(m_Resolution);

		if (m_RenderBuffer)
			m_RenderBuffer->Resize(m_Resolution.x, m_Resolution.y);
	}

	void Renderer::RenderWorld(const Ref<PerspectiveCamera>& camera, const Ref<World>& world)
	{
		SetResolution(camera->GetResolution());
		BeginScene();


		if (!m_RenderBuffer)
		{
			FramebufferSpecs bufferSpecs;
			bufferSpecs.width = m_Resolution.x;
			bufferSpecs.height = m_Resolution.y;
			bufferSpecs.attachments = {
				FramebufferTextureFormat::RGBA8,
				FramebufferTextureFormat::RED_INTEGER,
				FramebufferTextureFormat::DEPTH24STENCIL8
			};

			m_RenderBuffer = FramebufferManager::CreateFramebuffer("RenderBuffer");
			m_RenderBuffer->Configure(bufferSpecs);
		}

		const auto transformComponentArray = world->GetComponentArray<TransformComponent>();
		for (Entity entity = 0; entity < world->GetEntityCount(); entity++)
		{
			auto cTransform = transformComponentArray->GetData(entity);
			ProcessMeshComponent(entity, cTransform, world);
			ProcessLightComponent(entity, cTransform, world);
			ProcessBillboardComponent(entity, cTransform, world);
		}

		RenderPassParams renderPassParams;
		renderPassParams.world = world;
		renderPassParams.camera = camera;
		renderPassParams.meshCommands = m_MeshRenderCmds;

		m_GeometryPass.Render(renderPassParams);

		// Shadow Map Pass
		{
			ShadowMapPass::ShadowMapPassData shadowMapPassData = {m_DirectionalLights, m_SpotLights, m_PointLights};
			RenderPassParams shadowMapRenderPassParams = renderPassParams;
			shadowMapRenderPassParams.additionalData = &shadowMapPassData;

			m_ShadowMapPass.Render(shadowMapRenderPassParams);
		}


		m_RenderBuffer->Bind();
		RenderCommand::SetClearColor(m_RenderBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();
		m_RenderBuffer->ClearAttachment(1, -1);
		m_RenderBuffer->Unbind();

		// Sky Pass
		{
			auto skyComponents = world->GetComponentsByType<AtmosphericsComponent>();

			if (!skyComponents.empty())
			{
				auto cAtmos = skyComponents[0];

				SkyPass::SkyPassData skyPassData;
				skyPassData.targetBuffer = m_RenderBuffer;
				skyPassData.time = cAtmos.time;

				RenderPassParams skyPassRenderParams = renderPassParams;
				skyPassRenderParams.additionalData = &skyPassData;

				m_SkyPass.Render(skyPassRenderParams);
			}
		}

		// Lighting Pass
		{
			LightingPass::LightingPassData lightingPassData =
			{
				m_ShadowMapPass.GetShadowBuffer(),
				m_ShadowMapPass.GetPointShadowBuffer(),
				m_DirectionalLights,
				m_SpotLights,
				m_PointLights,
				m_RenderBuffer
			};

			RenderPassParams lightingPassRenderPassParams = renderPassParams;
			lightingPassRenderPassParams.additionalData = &lightingPassData;

			m_LightingPass.Render(lightingPassRenderPassParams);
		}

		// Billboard Pass
		{
			BillboardPass::BillboardPassData billboardPassData;
			billboardPassData.targetBuffer = m_RenderBuffer;
			billboardPassData.billboardCommands = m_BillboardRenderCmds;

			RenderPassParams billboardRenderPassParams = renderPassParams;
			billboardRenderPassParams.additionalData = &billboardPassData;

			m_BillboardPass.Render(billboardRenderPassParams);
		}

		// Screen Space Reflection Pass
		{
			SsrPass::SsrPassData ssrPassData;
			ssrPassData.gBuffer = m_GeometryPass.GetGBuffer();
			ssrPassData.renderTexture = m_RenderBuffer->GetColorAttachments()[0];

			RenderPassParams ssrRenderPassParams = renderPassParams;
			ssrRenderPassParams.additionalData = &ssrPassData;

			m_SsrPass.Render(ssrRenderPassParams);
		}

		/*
		{
			BoxBlurPass::BoxBlurPassData boxBlurPassData;
			boxBlurPassData.targetBuffer = m_SsrPass.GetFramebuffer();
			boxBlurPassData.colorTexture = m_SsrPass.GetFramebuffer()->GetColorAttachments()[0];

			RenderPassParams boxBlurPassRenderParams = renderPassParams;
			boxBlurPassRenderParams.additionalData = &boxBlurPassData;

			m_BoxBlurPass.Render(boxBlurPassRenderParams);
		}
		*/

		prevDrawCount = RenderCommand::GetDrawCallCount();
		EndScene();
	}

	void Renderer::ProcessMeshComponent(const Entity entity, const TransformComponent& cTransform,
	                                    const Ref<World>& world)
	{
		const auto meshComponentArray = world->GetComponentArray<MeshComponent>();
		if (!meshComponentArray->HasData(entity)) return;

		const auto cMesh = meshComponentArray->GetData(entity);
		if (!cMesh.m_Mesh) return;

		for (const Ref<SubMesh>& submesh : cMesh.m_Mesh->GetSubmeshes())
		{
			const Ref<Material> material = cMesh.m_Mesh->GetMaterials()[submesh->materialIndex].material;
			if (!material) continue;

			MeshCommand cmd;
			cmd.id = entity;
			cmd.transform = cTransform.GetModelMatrix();
			cmd.vertexArray = submesh->vertexArray;
			cmd.material = material;

			m_MeshRenderCmds.push_back(cmd);
		}
	}

	void Renderer::ProcessLightComponent(
		const Entity entity,
		const TransformComponent& cTransform,
		const Ref<World>& world)
	{
		const auto lightComponentArray = world->GetComponentArray<LightComponent>();
		if (!lightComponentArray->HasData(entity)) return;

		const auto cLight = lightComponentArray->GetData(entity);
		switch (cLight.m_Type)
		{
		case LightType::Directional:
			m_DirectionalLights.push_back(Utils::LightComponentToDirectionalLight(cTransform, cLight));
			break;
		case LightType::Point:
			m_PointLights.push_back(Utils::LightComponentToPointLight(cTransform, cLight));
			break;
		case LightType::Spot:
			m_SpotLights.push_back(Utils::LightComponentToSpotLight(cTransform, cLight));
			break;
		}
	}

	void Renderer::ProcessBillboardComponent(const Entity entity, const TransformComponent& cTransform,
	                                         const Ref<World>& world)
	{
		const auto billboardComponentArray = world->GetComponentArray<BillboardComponent>();
		if (!billboardComponentArray->HasData(entity)) return;

		const auto cBillboard = billboardComponentArray->GetData(entity);
		if (!cBillboard.billboardTexture) return;

		BillboardCommand cmd;
		cmd.id = entity;
		cmd.scale = 0.75f;
		cmd.transform = cTransform.GetModelMatrix();
		cmd.tintColor = cBillboard.tintColor;
		cmd.texture = cBillboard.billboardTexture;

		m_BillboardRenderCmds.push_back(cmd);
	}
}
