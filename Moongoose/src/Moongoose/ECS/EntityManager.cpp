#include "mgpch.h"
#include "EntityManager.h"

namespace Moongoose {
	
	EntityManager::EntityManager()
	{
		for (Entity entity = 0; entity < MAX_ENTITIES; ++entity)
		{
			m_AvailableEntities.push(entity);
		}
	}

	Entity EntityManager::CreateEntity()
	{
		MG_ASSERT(m_LivingEntityCount < MAX_ENTITIES, "Max number of entities exceeded!")

		Entity e = m_AvailableEntities.front();
		m_AvailableEntities.pop();
		++m_LivingEntityCount;
		return e;
	}

	void EntityManager::DestroyEntity(Entity entity)
	{
		MG_ASSERT(entity < MAX_ENTITIES, "Entity out of range");

		m_Signatures[entity].reset();
		m_AvailableEntities.push(entity);
		--m_LivingEntityCount;
	}

	void EntityManager::SetSignature(Entity entity, Signature signature)
	{
		MG_ASSERT(entity < MAX_ENTITIES, "Entity out of range");

		m_Signatures[entity] = signature;
	}

	Signature EntityManager::GetSignature(Entity entity)
	{
		MG_ASSERT(entity < MAX_ENTITIES, "Entity out of range");

		return m_Signatures[entity];
	}

}