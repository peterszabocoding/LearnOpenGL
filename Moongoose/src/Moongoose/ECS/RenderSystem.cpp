#include "mgpch.h"
#include "RenderSystem.h"

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Moongoose/Renderer/Renderer.h"

namespace Moongoose {

	void RenderSystem::Run(Ref<Scene> scene, Ref<PerspectiveCamera> camera)
	{
		for (Ref<Entity> e : scene->getEntities())
		{
			if (e->m_MeshComponent)
			{
				Ref<Shader> shader = e->m_MeshComponent->m_Shader;
				Ref<Mesh> mesh = e->m_MeshComponent->m_Mesh;

				shader->Bind();
				shader->SetCamera(
					camera->getCameraPosition(),
					camera->getViewMatrix(),
					camera->getProjection()
				);

				shader->SetModelTransform(getModelMatrix(e->m_TransformComponent));
				e->m_MeshComponent->m_Texture->bind(0);
				Renderer::RenderMesh(mesh->GetVertexArray());

				shader->Unbind();
			}
		}
	}

	 glm::mat4 RenderSystem::getModelMatrix(Ref<TransformComponent> component)
	{
		return glm::translate(glm::mat4(1.0f), component->m_Position)
			* glm::toMat4(glm::quat(glm::radians(component->m_Rotation)))
			* glm::scale(glm::mat4(1.0f), component->m_Scale);
	}
}