#include "mgpch.h"
#include "RenderSystem.h"

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Moongoose/Renderer/Renderer.h"
#include "Moongoose/ECS/EntityManager.h"

namespace Moongoose {

	void RenderSystem::Run(Ref<PerspectiveCamera> camera)
	{
		auto entites = EntityManager::Get().getEntities();

		for (auto e : entites)
		{
			if (EntityManager::Get().hasComponent<TransformComponent>(e) && EntityManager::Get().hasComponent<MeshComponent>(e))
			{
				TransformComponent& cTransform = EntityManager::Get().getComponent<TransformComponent>(e);
				MeshComponent& cMesh = EntityManager::Get().getComponent<MeshComponent>(e);

				cMesh.m_Shader->Bind();
				cMesh.m_Shader->SetCamera(
					camera->getCameraPosition(),
					camera->getViewMatrix(),
					camera->getProjection()
				);

				cMesh.m_Shader->SetModelTransform(getModelMatrix(cTransform));
				cMesh.m_Texture->bind(0);
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