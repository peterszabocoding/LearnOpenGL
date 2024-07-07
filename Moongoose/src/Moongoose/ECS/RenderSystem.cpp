#include "mgpch.h"
#include "RenderSystem.h"

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Moongoose/Renderer/Renderer.h"
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

	Signature RenderSystem::GetSystemSignature(Moongoose::World* world)
	{
		Signature signature;
		signature.set(world->GetComponentType<TransformComponent>());
		signature.set(world->GetComponentType<MeshComponent>());

		return signature;
	}

	void RenderSystem::Run(const Ref<PerspectiveCamera>& camera, Ref<World> world)
	{
		LightComponent directionalLightComponent;
		TransformComponent directionalLightTransformComponent;

		std::vector<LightComponent> lightComponents = world->GetComponentsByType<LightComponent>();
		for (size_t i = 0; i < lightComponents.size(); i++)
		{
			LightComponent cLight = lightComponents[i];
			if (cLight.m_Type == LightType::DIRECTIONAL)
			{
				directionalLightComponent = cLight;
				directionalLightTransformComponent = world->GetComponent<TransformComponent>(i);
			}
		}

		for (auto const& entity : m_Entities)
		{
			TransformComponent transformComponent = world->GetComponent<TransformComponent>(entity);
			MeshComponent meshComponent = world->GetComponent<MeshComponent>(entity);

			if (!meshComponent.m_Mesh) continue;

			std::vector<Ref<Material>> materials = meshComponent.m_Mesh->GetMaterials();
			for (auto& mat : materials)
			{
				Ref<Shader> shader = ShaderManager::GetShaderByType(mat->getShaderType());
				shader->Bind();
				shader->SetCamera(camera->getCameraPosition(), camera->getViewMatrix(), camera->getProjection());
				shader->SetDirectionalLight(
					directionalLightTransformComponent.getTransform(),
					directionalLightComponent.m_Color,
					directionalLightComponent.m_Intensity);

				shader->SetModelTransform(TransformComponent::GetModelMatrix(transformComponent));
				shader->SetEntityID(entity);
				shader->Unbind();
			}

			for (const Ref<SubMesh>& submesh : meshComponent.m_Mesh->GetSubmeshes())
			{
				Ref<Material> material = materials[submesh->materialIndex];
				Ref<Shader> shader = ShaderManager::GetShaderByType(material->getShaderType());

				shader->Bind();
				material->bind();
				Renderer::RenderMesh(submesh->vertexArray);
				shader->Unbind();
			}
		}
	}
}