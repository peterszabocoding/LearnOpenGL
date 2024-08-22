#include "mgpch.h"
#include "Renderer.h"

#include "MeshPrimitives.h"
#include "RenderCommand.h"
#include "ShaderManager.h"
#include "Moongoose/ECS/World.h"
#include "Moongoose/ECS/Systems/AtmosphericsSystem.h"
#include "Moongoose/Renderer/Framebuffer.h"
#include "Light.h"

namespace Moongoose
{
	glm::uvec2 Renderer::m_Resolution;

	SsrPass Renderer::m_SsrPass;
	LightingPass Renderer::m_LightingPass;
	GeometryPass Renderer::m_GeometryPass;
	ShadowMapPass Renderer::m_ShadowMapPass;

	std::vector<DirectionalLight> Renderer::m_DirectionalLights;
	std::vector<PointLight> Renderer::m_PointLights;
	std::vector<SpotLight> Renderer::m_SpotLights;

	std::vector<MeshCommand> Renderer::m_MeshRenderCmds;
	std::vector<BillboardCommand> Renderer::m_BillboardRenderCmds;

	unsigned int Renderer::prevDrawCount = 0;
	unsigned int Renderer::currentDrawCount = 0;


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
		prevDrawCount = currentDrawCount;
		currentDrawCount = 0;

		m_DirectionalLights.clear();
		m_PointLights.clear();
		m_SpotLights.clear();

		m_MeshRenderCmds.clear();
		m_BillboardRenderCmds.clear();
	}

	// Init
	void Renderer::SetResolution(const glm::uvec2 newResolution)
	{
		if (newResolution == m_Resolution) return;
		m_Resolution = newResolution;

		m_GeometryPass.Resize(m_Resolution);
		m_LightingPass.Resize(m_Resolution);
		m_SsrPass.Resize(m_Resolution);
	}

	// Rendering
	void Renderer::RenderWorld(const Ref<PerspectiveCamera>& camera, const Ref<World>& world)
	{
		const auto transformComponentArray = world->GetComponentArray<TransformComponent>();
		const auto meshComponentArray = world->GetComponentArray<MeshComponent>();
		const auto lightComponentArray = world->GetComponentArray<LightComponent>();
		const auto billboardComponentArray = world->GetComponentArray<BillboardComponent>();

		SetResolution(camera->GetResolution());
		BeginScene();

		world->GetSystem<AtmosphericsSystem>()->Update(camera, m_Resolution);

		for (size_t i = 0; i < transformComponentArray->m_Size; i++)
		{
			const Entity entity = transformComponentArray->m_IndexToEntityMap[i];
			auto cTransform = transformComponentArray->GetData(entity);

			if (meshComponentArray->HasData(entity))
			{
				const auto cMesh = meshComponentArray->GetData(entity);
				if (cMesh.m_Mesh)
				{
					for (const Ref<SubMesh>& submesh : cMesh.m_Mesh->GetSubmeshes())
					{
						const Ref<Material> material = cMesh.m_Mesh->GetMaterials()[submesh->materialIndex].material;
						if (!material) continue;

						MeshCommand cmd = {entity, cTransform.GetModelMatrix(), submesh->vertexArray, material};
						m_MeshRenderCmds.push_back(cmd);
					}
				}
			}

			if (lightComponentArray->HasData(entity))
			{
				switch (const auto cLight = lightComponentArray->GetData(entity); cLight.m_Type)
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

			if (billboardComponentArray->HasData(entity))
			{
				const auto billboardComponent = billboardComponentArray->GetData(entity);
				if (!billboardComponent.m_BillboardTexture) continue;

				BillboardCommand cmd = {
					entity,
					cTransform.GetModelMatrix(),
					billboardComponent.m_BillboardTexture,
					0.75f,
					billboardComponent.m_TintColor
				};
				m_BillboardRenderCmds.push_back(cmd);
			}
		}

		RenderPassParams renderPassParams;
		renderPassParams.world = world;
		renderPassParams.camera = camera;
		renderPassParams.meshCommands = m_MeshRenderCmds;

		m_GeometryPass.Render(renderPassParams);


		ShadowMapPass::ShadowMapPassData shadowMapPassData = {m_DirectionalLights, m_SpotLights, m_PointLights};

		RenderPassParams shadowMapRenderPassParams = renderPassParams;
		shadowMapRenderPassParams.additionalData = &shadowMapPassData;

		m_ShadowMapPass.Render(shadowMapRenderPassParams);


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


		SsrPass::SsrPassData ssrPassData;
		ssrPassData.gBuffer = m_GeometryPass.GetGBuffer();
		ssrPassData.renderTexture = m_LightingPass.GetFramebuffer()->GetColorAttachments()[0];

		RenderPassParams ssrRenderPassParams = renderPassParams;
		ssrRenderPassParams.additionalData = &ssrPassData;

		m_SsrPass.Render(ssrRenderPassParams);


		for (const auto& cmd : m_BillboardRenderCmds)
			ExecuteBillboardRenderCommand(camera, cmd);


		EndScene();
	}

	void Renderer::RenderMesh(const Ref<VertexArray>& vertexArray)
	{
		RenderCommand::DrawIndexed(vertexArray);
		currentDrawCount++;
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
}
