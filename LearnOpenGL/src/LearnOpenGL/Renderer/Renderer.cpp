#include "pch.h"
#include "Renderer.h"

#include "FramebufferManager.h"
#include "GBuffer.h"
#include "Light.h"
#include "MeshPrimitives.h"
#include "RenderCommand.h"
#include "LearnOpenGL/ECS/World.h"
#include "LearnOpenGL/Renderer/Framebuffer.h"
#include "LearnOpenGL/ECS/Components.h"

namespace LearnOpenGL
{
	glm::uvec2 Renderer::m_Resolution;

	Ref<Framebuffer> Renderer::m_RenderBuffer;
	Ref<Framebuffer> Renderer::m_PostProcessingSSRBuffer;
	Ref<GBuffer> Renderer::m_GBuffer;

	PostProcessingSSRPass Renderer::m_SsrPass;
	LightingPass Renderer::m_LightingPass;
	GeometryPass Renderer::m_GeometryPass;
	ShadowMapPass Renderer::m_ShadowMapPass;
	BillboardPass Renderer::m_BillboardPass;
	BoxBlurPass Renderer::m_BoxBlurPass;
	SkyPass Renderer::m_SkyPass;
	PostProcessCombinePass Renderer::m_PostProcessCombinePass;

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
		m_GBuffer->Bind();
		const int entityId = m_GBuffer->ReadPixel(4, x, y);
		m_GBuffer->Unbind();
		return entityId;
	}

	void Renderer::SetResolution(const glm::uvec2 newResolution)
	{
		if (newResolution == m_Resolution) return;
		m_Resolution = newResolution;

		if (m_GBuffer) m_GBuffer->Resize(m_Resolution.x, m_Resolution.y);
		if (m_RenderBuffer) m_RenderBuffer->Resize(m_Resolution.x, m_Resolution.y);
		if (m_PostProcessingSSRBuffer) m_PostProcessingSSRBuffer->Resize(m_Resolution.x, m_Resolution.y);
	}

	void Renderer::CreateSSRBuffer()
	{
		FramebufferSpecs bufferSpecs;
		bufferSpecs.width = m_Resolution.x;
		bufferSpecs.height = m_Resolution.y;
		bufferSpecs.attachments = {
			FramebufferTextureFormat::RGBA8
		};
		bufferSpecs.useNearestFilter = true;

		m_PostProcessingSSRBuffer = FramebufferManager::CreateFramebuffer("PostProcessingSSRBuffer");
		m_PostProcessingSSRBuffer->Configure(bufferSpecs);
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
				FramebufferTextureFormat::DEPTH24STENCIL8
			};

			m_RenderBuffer = FramebufferManager::CreateFramebuffer("RenderBuffer");
			m_RenderBuffer->Configure(bufferSpecs);
		}

		if (!m_GBuffer)
		{
			m_GBuffer = CreateRef<GBuffer>(GBuffer::GBufferSpecs({m_Resolution.x, m_Resolution.y}));
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

		// Clear entity ID buffer
		m_GBuffer->GetFramebuffer()->Bind();
		m_GBuffer->GetFramebuffer()->ClearAttachment(4, -1);
		m_GBuffer->GetFramebuffer()->Unbind();

		// Clear render buffer
		m_RenderBuffer->Bind();
		RenderCommand::SetClearColor(m_RenderBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();
		m_RenderBuffer->Unbind();

		// Geometry Pass
		{
			m_GeometryPass.Render(m_GBuffer, renderPassParams);
		}

		// Shadow Map Pass
		{
			ShadowMapPass::ShadowMapPassData shadowMapPassData = {m_DirectionalLights, m_SpotLights, m_PointLights};
			RenderPassParams shadowMapRenderPassParams = renderPassParams;
			shadowMapRenderPassParams.additionalData = &shadowMapPassData;

			m_ShadowMapPass.Render(nullptr, shadowMapRenderPassParams);
		}

		// Sky Pass
		{
			auto skyComponents = world->GetComponentsByType<AtmosphericsComponent>();
			if (!skyComponents.empty())
			{
				auto& cAtmos = skyComponents[0];

				SkyPass::SkyPassData skyPassData{cAtmos.time};
				RenderPassParams skyPassRenderParams = renderPassParams;

				skyPassRenderParams.additionalData = &skyPassData;

				m_SkyPass.Render(m_RenderBuffer, skyPassRenderParams);
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
				m_PointLights
			};

			RenderPassParams lightingPassRenderPassParams = renderPassParams;
			lightingPassRenderPassParams.additionalData = &lightingPassData;

			m_LightingPass.Render(m_RenderBuffer, lightingPassRenderPassParams);
		}

		// Billboard Pass
		{
			BillboardPass::BillboardPassData billboardPassData;
			billboardPassData.billboardCommands = m_BillboardRenderCmds;

			RenderPassParams billboardRenderPassParams = renderPassParams;
			billboardRenderPassParams.additionalData = &billboardPassData;

			m_BillboardPass.Render(m_RenderBuffer, billboardRenderPassParams);
		}

		// Screen Space Reflection Pass
		{
			auto postProcessComponent = world->GetComponentsByType<PostProcessingVolumeComponent>();

			if (!postProcessComponent.empty())
			{
				if (!m_PostProcessingSSRBuffer) CreateSSRBuffer();

				PostProcessingSSRPass::PostProcessingSSRPassData ssrPassData;
				ssrPassData.gBuffer = m_GBuffer;
				ssrPassData.renderTexture = m_RenderBuffer->GetColorAttachments()[0];

				auto params = PostProcessingSSRPass::PostProcessingSSRParams();
				params.intensity = postProcessComponent[0].SSR_Intensity;
				params.blur = postProcessComponent[0].SSR_Blur;
				params.maxDistance = postProcessComponent[0].SSR_maxDistance;
				params.thickness = postProcessComponent[0].SSR_thickness;
				params.resolution = postProcessComponent[0].SSR_resolution;
				params.steps = postProcessComponent[0].SSR_steps;

				ssrPassData.ssrParams = params;

				RenderPassParams ssrRenderPassParams = renderPassParams;
				ssrRenderPassParams.additionalData = &ssrPassData;

				m_SsrPass.Render(m_PostProcessingSSRBuffer, ssrRenderPassParams);
				m_BoxBlurPass.Render(m_PostProcessingSSRBuffer, m_PostProcessingSSRBuffer->GetColorAttachments()[0], 1,
				                     params.blur);

				m_PostProcessCombinePass.Render(
					m_RenderBuffer,
					m_RenderBuffer->GetColorAttachments()[0],
					m_PostProcessingSSRBuffer->GetColorAttachments()[0],
					params.intensity);
			}
		}

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
