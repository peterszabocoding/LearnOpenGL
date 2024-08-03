#include "mgpch.h"
#include "BillboardSystem.h"

#define GLM_ENABLE_EXPERIMENTAL

#include "Moongoose/Renderer/Renderer.h"
#include "Moongoose/ECS/Components.h"
#include "Moongoose/Renderer/MeshPrimitives.h"
#include "Moongoose/Renderer/ShaderManager.h"

namespace Moongoose
{
	BillboardSystem::BillboardSystem()
	{
		m_DefaultMaterial = CreateRef<Material>("DefaultMaterial");
	}

	Signature BillboardSystem::GetSystemSignature(World* world)
	{
		Signature signature;
		signature.set(world->GetComponentType<TransformComponent>());
		signature.set(world->GetComponentType<BillboardComponent>());

		return signature;
	}

	void BillboardSystem::Run(const Ref<PerspectiveCamera>& camera, const Ref<World>& world) const
	{
		for (auto const& entity : m_Entities)
		{
			auto transformComponent = world->GetComponent<TransformComponent>(entity);
			const auto billboardComponent = world->GetComponent<BillboardComponent>(entity);

			if (!billboardComponent.m_BillboardTexture) continue;

			const Ref<Shader> shader = ShaderManager::GetShaderByType(ShaderType::BILLBOARD);
			shader->Bind();
			shader->SetCamera(camera->GetCameraPosition(), camera->GetViewMatrix(), camera->GetProjection());
			shader->SetModelTransform(TransformComponent::GetModelMatrix(transformComponent));
			shader->SetEntityId(entity);
			shader->SetBlendMode(true);

			shader->UploadUniformFloat3("TintColor", billboardComponent.m_ColorTint);

			billboardComponent.m_BillboardTexture->bind(0);
			Renderer::RenderMesh(QuadMeshWorld(0.75f).GetSubmeshes()[0]->vertexArray);

			shader->SetBlendMode(false);
			shader->Unbind();
		}
	}
}
