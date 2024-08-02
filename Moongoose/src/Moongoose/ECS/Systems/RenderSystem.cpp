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
			case LightType::Directional:
				return glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -25.0f, 25.0f);
			case LightType::Point:
				return glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, -25.0f, 25.0f);
			case LightType::Spot:
				return glm::perspective(30.0f, (GLfloat) 4096 / (GLfloat) 4096, 0.1f, 1000.0f);
			default:
				MG_ASSERT(false, "Unknown light type")
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

	void RenderSystem::Run(const Ref<PerspectiveCamera>& camera, const Ref<World>& world) const
	{
		for (auto const& entity : m_Entities)
		{
			auto transformComponent = world->GetComponent<TransformComponent>(entity);
			const auto meshComponent = world->GetComponent<MeshComponent>(entity);

			if (!meshComponent.m_Mesh) continue;

			std::vector<MaterialSlot> materials = meshComponent.m_Mesh->GetMaterials();
			for (auto& [materialName, material] : materials)
			{
				if (!material) continue;

				const Ref<Shader> shader = ShaderManager::GetShaderByType(material->GetShaderType());
				shader->Bind();
				shader->SetCamera(camera->GetCameraPosition(), camera->GetViewMatrix(), camera->GetProjection());
				shader->SetModelTransform(TransformComponent::GetModelMatrix(transformComponent));
				shader->SetEntityID(entity);
				shader->Unbind();
			}

			for (const Ref<SubMesh>& submesh : meshComponent.m_Mesh->GetSubmeshes())
			{
				const Ref<Material> material = materials[submesh->materialIndex].material;
				if (!material) continue;

				const Ref<Shader> shader = ShaderManager::GetShaderByType(material->GetShaderType());

				shader->Bind();
				material->Bind();
				Renderer::RenderMesh(submesh->vertexArray);
				shader->Unbind();
			}
		}
	}
} 