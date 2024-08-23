#include "mgpch.h"
#include "Renderer.h"

#include "Light.h"
#include "MeshPrimitives.h"
#include "RenderCommand.h"
#include "Moongoose/ECS/World.h"
#include "Moongoose/Renderer/Framebuffer.h"
#include "Moongoose/ECS/Components.h"
#include "Moongoose/ECS/Systems/AtmosphericsSystem.h"

namespace Moongoose
{
	glm::uvec2 Renderer::m_Resolution;

	SsrPass Renderer::m_SsrPass;
	LightingPass Renderer::m_LightingPass;
	GeometryPass Renderer::m_GeometryPass;
	ShadowMapPass Renderer::m_ShadowMapPass;
	BillboardPass Renderer::m_BillboardPass;

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

	void Renderer::SetResolution(const glm::uvec2 newResolution)
	{
		if (newResolution == m_Resolution) return;
		m_Resolution = newResolution;

		m_GeometryPass.Resize(m_Resolution);
		m_LightingPass.Resize(m_Resolution);
		m_SsrPass.Resize(m_Resolution);
	}

	void Renderer::RenderWorld(const Ref<PerspectiveCamera>& camera, const Ref<World>& world)
	{
		SetResolution(camera->GetResolution());
		BeginScene();

		world->GetSystem<AtmosphericsSystem>()->Update(camera, m_Resolution);

		const auto transformComponentArray = world->GetComponentArray<TransformComponent>();
		for (size_t i = 0; i < transformComponentArray->m_Size; i++)
		{
			const Entity entity = transformComponentArray->m_IndexToEntityMap[i];
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

			m_LightingPass.Render(lightingPassRenderPassParams);
		}

		// Billboard Pass
		{
			BillboardPass::BillboardPassData billboardPassData;
			billboardPassData.targetBuffer = m_LightingPass.GetFramebuffer();
			billboardPassData.billboardCommands = m_BillboardRenderCmds;

			RenderPassParams billboardRenderPassParams = renderPassParams;
			billboardRenderPassParams.additionalData = &billboardPassData;

			m_BillboardPass.Render(billboardRenderPassParams);
		}

		// Screen Space Reflection Pass
		{
			SsrPass::SsrPassData ssrPassData;
			ssrPassData.gBuffer = m_GeometryPass.GetGBuffer();
			ssrPassData.renderTexture = m_LightingPass.GetFramebuffer()->GetColorAttachments()[0];

			RenderPassParams ssrRenderPassParams = renderPassParams;
			ssrRenderPassParams.additionalData = &ssrPassData;

			m_SsrPass.Render(ssrRenderPassParams);
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

	void Renderer::ProcessLightComponent(const Entity entity, const TransformComponent& cTransform,
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
