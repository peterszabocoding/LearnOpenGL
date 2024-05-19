#include "mgpch.h"
#include "RenderSystem.h"

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Moongoose/Renderer/Renderer.h"
#include "Moongoose/ECS/EntityManager.h"
#include "Moongoose/ECS/Components.h"
#include "Moongoose/Renderer/MeshPrimitives.h"
#include "Moongoose/Renderer/ShaderManager.h"

namespace Moongoose {

	namespace Utils {

		static glm::mat4 GetLightProjection(LightType type) {
			switch (type) {
			case LightType::DIRECTIONAL:
				return glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -25.0f, 25.0f);
			case LightType::POINT:
				return glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -25.0f, 25.0f);
			case LightType::SPOT:
				return glm::perspective(30.0f, (GLfloat)4096 / (GLfloat)4096, 0.1f, 1000.0f);
			default:
				MG_ASSERT(false, "Unknown light type");
				return glm::mat4(1.0);
			}
		}
	}

	void RenderSystem::Run(const Ref<PerspectiveCamera>& camera, Ref<World> world)
	{
		LightComponent directionalLightComponent;
		TransformComponent directionalLightTransformComponent;

		auto& lightComponents = world->GetComponents<LightComponent>();
		for (size_t i = 0; i < lightComponents.size(); i++)
		{
			auto& cLight = lightComponents[i];
			if (!cLight.m_Active) continue;

			if (cLight.m_Type == LightType::DIRECTIONAL)
			{
				directionalLightComponent = cLight;
				directionalLightTransformComponent = world->GetComponent<TransformComponent>(i);
			}
		}

		auto& meshComponents = world->GetComponents<MeshComponent>();
		for (size_t i = 0; i < meshComponents.size(); i++)
		{
			MeshComponent& cMesh = meshComponents[i];
			if (!cMesh.m_Active || !cMesh.m_Mesh) continue;

			TransformComponent& cTransform = world->GetComponent<TransformComponent>(i);
			std::vector<Ref<Material>> materials = cMesh.m_Mesh->GetMaterials();

			for (auto& mat : materials)
			{
				auto& shader = ShaderManager::Get().GetShaderByType(mat->getShaderType());
				shader->Bind();
				shader->SetCamera(camera->getCameraPosition(), camera->getViewMatrix(), camera->getProjection());
				shader->SetDirectionalLight(
					directionalLightTransformComponent.getTransform(),
					directionalLightComponent.m_Color,
					directionalLightComponent.m_Intensity);

				shader->SetModelTransform(TransformComponent::GetModelMatrix(cTransform));
				shader->SetEntityID(i);
				shader->Unbind();
			}

			for (const Ref<SubMesh>& submesh : cMesh.m_Mesh->GetSubmeshes())
			{
				auto& mat = materials[submesh->materialIndex];
				auto& shader = ShaderManager::Get().GetShaderByType(mat->getShaderType());
				
				shader->Bind();
				mat->bind();
				Renderer::RenderMesh(submesh->vertexArray);
				shader->Unbind();
			}

			/*
			if (cMesh.m_DebugShader)
			{
				cMesh.m_DebugShader->Bind();
				cMesh.m_DebugShader->SetCamera(
					m_RenderCamera->getCameraPosition(),
					m_RenderCamera->getViewMatrix(),
					m_RenderCamera->getProjection()
				);

				TransformComponent& cTransform = EntityManager::Get().getComponent<TransformComponent>(i);
				cMesh.m_DebugShader->SetModelTransform(getModelMatrix(cTransform));

				Renderer::RenderMesh(BoundingBox(
					cMesh.m_Mesh->GetBounds().pMin,
					cMesh.m_Mesh->GetBounds().pMax)->GetVertexArray());

				cMesh.m_DebugShader->Unbind();
			}
			
			*/
			
		}
	}
}