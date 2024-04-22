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

	void RenderSystem::SetCamera(Ref<PerspectiveCamera> camera)
	{
		m_RenderCamera = camera;
	}

	Ref<PerspectiveCamera> RenderSystem::GetCamera()
	{
		return m_RenderCamera;
	}

	void RenderSystem::Run()
	{
		auto& entites = EntityManager::Get().getEntities();
		LightComponent directionalLightComponent;
		TransformComponent directionalLightTransformComponent;

		auto& lightComponents = EntityManager::Get().getComponents<LightComponent>();
		for (size_t i = 0; i < lightComponents.size(); i++)
		{
			if (lightComponents[i].m_Active)
			{
				directionalLightComponent = lightComponents[i];
				directionalLightTransformComponent = EntityManager::Get().getComponent<TransformComponent>(i);
				break;
			}
		}

		auto& meshComponents = EntityManager::Get().getComponents<MeshComponent>();
		for (size_t i = 0; i < meshComponents.size(); i++)
		{
			MeshComponent& cMesh = meshComponents[i];
			if (!cMesh.m_Active || !cMesh.m_Mesh) continue;
			
			cMesh.m_Shader->Bind();
			cMesh.m_Shader->SetCamera(
				m_RenderCamera->getCameraPosition(),
				m_RenderCamera->getViewMatrix(),
				m_RenderCamera->getProjection()
			);

			glm::vec4 lightDirection = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
			glm::vec3 localUp = glm::vec3(0.0f, 1.0f, 0.0f);

			glm::vec3 direction = glm::vec3(directionalLightTransformComponent.getTransform() * lightDirection);
			glm::mat4 projection = Utils::GetLightProjection(directionalLightComponent.m_Type);

			glm::mat4 lightTransform = projection * glm::lookAt(-direction, glm::vec3(0.0f), localUp);

			cMesh.m_Shader->SetDirectionalLight(
				lightTransform,
				direction,
				directionalLightComponent.m_Color,
				directionalLightComponent.m_Intensity);

			TransformComponent& cTransform = EntityManager::Get().getComponent<TransformComponent>(i);
			cMesh.m_Shader->SetModelTransform(getModelMatrix(cTransform));
			cMesh.m_Shader->SetEntityID(i);
			cMesh.m_Material->bind();

			Renderer::RenderMesh(cMesh.m_Mesh->GetVertexArray());

			cMesh.m_Shader->Unbind();
		}
	}

	glm::mat4 RenderSystem::getModelMatrix(const TransformComponent& component)
	{
		return glm::translate(glm::mat4(1.0f), component.m_Position)
			* glm::toMat4(glm::quat(glm::radians(component.m_Rotation)))
			* glm::scale(glm::mat4(1.0f), component.m_Scale);
	}
}