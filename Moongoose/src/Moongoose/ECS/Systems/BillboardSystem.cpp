#include "mgpch.h"
#include "BillboardSystem.h"

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Moongoose/Renderer/Renderer.h"
#include "Moongoose/ECS/Components.h"
#include "Moongoose/Renderer/MeshPrimitives.h"
#include "Moongoose/Renderer/ShaderManager.h"

namespace Moongoose
{
	Signature BillboardSystem::GetSystemSignature(World* world)
	{
		Signature signature;
		signature.set(world->GetComponentType<TransformComponent>());
		signature.set(world->GetComponentType<BillboardComponent>());

		return signature;
	}

	void BillboardSystem::Run(const Ref<PerspectiveCamera>& camera, Ref<World> world)
	{
		for (auto const& entity : m_Entities)
		{
			TransformComponent transformComponent = world->GetComponent<TransformComponent>(entity);
			BillboardComponent billboardComponent = world->GetComponent<BillboardComponent>(entity);

			if (!billboardComponent.m_BillboardTexture) continue;

			Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::BILLBOARD);
			shader->Bind();
			shader->SetCamera(camera->getCameraPosition(), camera->getViewMatrix(), camera->getProjection());
			shader->SetModelTransform(TransformComponent::GetModelMatrix(transformComponent));
			shader->SetEntityID(entity);
			//shader->SetDepthTest(false);

			billboardComponent.m_BillboardTexture->bind(0);
			
			Renderer::RenderMesh(QuadMesh(0.75f).GetSubmeshes()[0]->vertexArray);

			//shader->SetDepthTest(true);
			shader->Unbind();
		}
	}
}
