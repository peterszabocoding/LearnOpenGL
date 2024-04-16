#include "mgpch.h"
#include "EntityManager.h"

#include "EntityMemoryPool.h"

namespace Moongoose {

	Entity EntityManager::addEntity(std::string tag)
	{
		Entity e = EntityMemoryPool::Get().addEntity(tag);
		return m_Entities.emplace_back(e);
	}

	const std::vector<Entity>& EntityManager::getEntities() const
	{
		return m_Entities;
	}

}
