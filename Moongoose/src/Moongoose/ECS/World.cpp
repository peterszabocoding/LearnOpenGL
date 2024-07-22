#include "mgpch.h"
#include "World.h"

namespace Moongoose {
	void World::Init()
	{
		m_ComponentManager = CreateScope<ComponentManager>();
		m_EntityManager = CreateScope<EntityManager>();
		m_SystemManager = CreateScope<SystemManager>();
	}

	Entity World::CreateEntity(const std::string& tag) const
	{
		Entity entity = m_EntityManager->CreateEntity();

		m_ComponentManager->AddComponent<IDComponent>(entity, IDComponent());
		m_ComponentManager->AddComponent<TagComponent>(entity, TagComponent(tag));
		m_ComponentManager->AddComponent<TransformComponent>(entity, TransformComponent());

		Signature signature = m_EntityManager->GetSignature(entity);
		signature.set(m_ComponentManager->GetComponentType<IDComponent>(), true);
		signature.set(m_ComponentManager->GetComponentType<TagComponent>(), true);
		signature.set(m_ComponentManager->GetComponentType<TransformComponent>(), true);
		m_EntityManager->SetSignature(entity, signature);

		m_SystemManager->EntitySignatureChanged(entity, signature);
		return entity;
	}

	void World::DestroyEntity(Entity entity)
	{
		if (entity == m_SelectedEntity) m_SelectedEntity = -1;

		m_EntityManager->DestroyEntity(entity);
		m_ComponentManager->EntityDestroyed(entity);
		m_SystemManager->EntityDestroyed(entity);
	}
}
