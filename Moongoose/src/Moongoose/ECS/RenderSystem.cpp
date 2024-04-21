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

		for (auto e : entites)
		{
			if (EntityManager::Get().hasComponent<TransformComponent>(e) && EntityManager::Get().hasComponent<MeshComponent>(e))
			{
				TransformComponent& cTransform = EntityManager::Get().getComponent<TransformComponent>(e);
				MeshComponent& cMesh = EntityManager::Get().getComponent<MeshComponent>(e);

				cMesh.m_Shader->Bind();
				cMesh.m_Shader->SetCamera(
					m_RenderCamera->getCameraPosition(),
					m_RenderCamera->getViewMatrix(),
					m_RenderCamera->getProjection()
				);

				cMesh.m_Shader->SetModelTransform(getModelMatrix(cTransform));
				cMesh.m_Shader->SetEntityID(e);
				cMesh.m_Material->bind();
				Renderer::RenderMesh(cMesh.m_Mesh->GetVertexArray());

				cMesh.m_Shader->Unbind();
			}
		}
	}

	glm::mat4 RenderSystem::getModelMatrix(const TransformComponent& component)
	{
		return glm::translate(glm::mat4(1.0f), component.m_Position)
			* glm::toMat4(glm::quat(glm::radians(component.m_Rotation)))
			* glm::scale(glm::mat4(1.0f), component.m_Scale);
	}
}