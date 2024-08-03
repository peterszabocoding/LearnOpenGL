#include "mgpch.h"
#include "RenderSystem.h"

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "Moongoose/ECS/Components.h"
#include "Moongoose/Renderer/Renderer.h"
#include "Moongoose/Renderer/MeshPrimitives.h"

namespace Moongoose {

	Signature RenderSystem::GetSystemSignature(World* world)
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
			auto cTransform = world->GetComponent<TransformComponent>(entity);
			const auto cMesh = world->GetComponent<MeshComponent>(entity);
			if (!cMesh.m_Mesh) continue;

			for (const Ref<SubMesh>& submesh : cMesh.m_Mesh->GetSubmeshes())
			{
				const Ref<Material> material = cMesh.m_Mesh->GetMaterials()[submesh->materialIndex].material;
				if (!material) continue;

				Renderer::MeshRenderCmd cmd = { entity, cTransform.GetModelMatrix(), submesh->vertexArray, material };
				Renderer::PushMeshRenderCommand(cmd);
			}
		}
	}
} 