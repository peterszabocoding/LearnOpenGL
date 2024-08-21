#include "mgpch.h"
#include "Renderer.h"

#include "FramebufferManager.h"
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

	SSRPass Renderer::m_SsrPass;
	GeometryPass Renderer::m_GeometryPass;
	ShadowMapPass Renderer::m_ShadowMapPass;

	Ref<Framebuffer> Renderer::m_RenderBuffer;

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

		m_RenderBuffer = FramebufferManager::CreateFramebuffer("RenderBuffer");
		m_RenderBuffer->Configure(specs);
	}

	void Renderer::SetResolution(const glm::uvec2 newResolution)
	{
		if (newResolution == m_Resolution) return;
		m_Resolution = newResolution;
		if (!m_RenderBuffer) InitRenderBuffer();
	}

	void Renderer::SetLights()
	{
		const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::STATIC);
		shader->Bind();

		shader->SetFloat("directionalLight.base.intensity", 0.0f);
		shader->SetFloat3("directionalLight.base.color", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		shader->SetFloat3("directionalLight.direction", glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));

		shader->BindTexture(4, m_ShadowMapPass.GetShadowBuffer()->GetShadowMapAttachmentID());
		shader->BindCubeMapTexture(5, m_ShadowMapPass.GetPointShadowBuffer()->GetShadowMapCubeAttachmentID());

		for (const auto& light : m_DirectionalLights) AddDirectionalLight(light, shader);

		shader->UploadUniformInt("pointLightCount", m_PointLights.size());
		for (size_t i = 0; i < m_PointLights.size(); i++) AddPointLight(i, shader, m_PointLights[i]);

		shader->UploadUniformInt("spotLightCount", m_SpotLights.size());
		for (size_t i = 0; i < m_SpotLights.size(); i++) AddSpotLight(i, shader, m_SpotLights[i]);

		shader->Unbind();
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
						PushMeshRenderCommand(cmd);
					}
				}
			}

			if (lightComponentArray->HasData(entity))
			{
				switch (const auto cLight = lightComponentArray->GetData(entity); cLight.m_Type)
				{
				case LightType::Directional:
					PushDirectionalLight(Utils::LightComponentToDirectionalLight(cTransform, cLight));
					break;
				case LightType::Point:
					PushPointLight(Utils::LightComponentToPointLight(cTransform, cLight));
					break;
				case LightType::Spot:
					PushSpotLight(Utils::LightComponentToSpotLight(cTransform, cLight));
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
				PushBillboardRenderCommand(cmd);
			}
		}

		ShadowMapPass::Params shadowSpecs = {
			m_Resolution,
			camera,
			m_MeshRenderCmds,
			m_DirectionalLights,
			m_SpotLights,
			m_PointLights
		};
		m_ShadowMapPass.Render(&shadowSpecs);

		GeometryPass::Specs gSpecs = {m_Resolution, camera, m_MeshRenderCmds};
		m_GeometryPass.Render(&gSpecs);

		m_RenderBuffer->Bind();
		RenderCommand::SetClearColor(m_RenderBuffer->GetSpecs().clearColor);
		RenderCommand::Clear();

		world->GetSystem<AtmosphericsSystem>()->Run(camera);

		m_RenderBuffer->ClearAttachment(1, -1);
		SetLights();

		for (const auto& cmd : m_MeshRenderCmds)
			ExecuteMeshRenderCommand(camera, cmd);

		for (const auto& cmd : m_BillboardRenderCmds)
			ExecuteBillboardRenderCommand(camera, cmd);

		m_RenderBuffer->Unbind();


		SSRPass::Params ssrParams = {
			m_Resolution, camera,
			m_GeometryPass.GetGBuffer(),
			m_RenderBuffer->GetColorAttachments()[0]
		};
		m_SsrPass.Render(&ssrParams);

		EndScene();
	}

	void Renderer::RenderMesh(const Ref<VertexArray>& vertexArray)
	{
		RenderCommand::DrawIndexed(vertexArray);
		currentDrawCount++;
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


	// Render commands

	void Renderer::PushMeshRenderCommand(const MeshCommand& cmd)
	{
		m_MeshRenderCmds.push_back(cmd);
	}

	void Renderer::PushBillboardRenderCommand(const BillboardCommand& cmd)
	{
		m_BillboardRenderCmds.push_back(cmd);
	}


	// Add lights

	void Renderer::PushDirectionalLight(const DirectionalLight& light)
	{
		m_DirectionalLights.push_back(light);
	}

	void Renderer::PushPointLight(const PointLight& light)
	{
		m_PointLights.push_back(light);
	}

	void Renderer::PushSpotLight(const SpotLight& light)
	{
		m_SpotLights.push_back(light);
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

		shader->BindTexture(4, m_ShadowMapPass.GetShadowBuffer()->GetShadowMapAttachmentID());

		if (light.shadowType != ShadowType::NONE && light.shadowMapRegion)
		{
			const auto& [topLeft, bottomRight] = *light.shadowMapRegion;
			const auto shadowMapSize = glm::vec2(bottomRight.x - topLeft.x, bottomRight.y - topLeft.y);

			shader->SetFloat2("directionalLight.base.shadowMapTopLeft", topLeft);
			shader->SetFloat2("directionalLight.base.shadowMapSize", shadowMapSize);
		}
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
