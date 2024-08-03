#include "mgpch.h"
#include "BillboardSystem.h"

#define GLM_ENABLE_EXPERIMENTAL

#include "Moongoose/Renderer/Renderer.h"
#include "Moongoose/ECS/Components.h"

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
			auto cTransform = world->GetComponent<TransformComponent>(entity);
			const auto billboardComponent = world->GetComponent<BillboardComponent>(entity);

			if (!billboardComponent.m_BillboardTexture) continue;

			Renderer::BillboardCmd cmd;
			cmd = {entity, cTransform.GetModelMatrix(), billboardComponent.m_BillboardTexture};
			cmd.tintColor = billboardComponent.m_TintColor;
			Renderer::PushBillboardRenderCommand(cmd);
		}
	}
}
