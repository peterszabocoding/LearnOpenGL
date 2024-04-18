#include "mgpch.h"
#include "EntityManager.h"

#include "EntityMemoryPool.h"

namespace Moongoose {

	Entity EntityManager::addEntity(std::string tag)
	{
		Entity e = EntityMemoryPool::Get().addEntity(tag);
		return m_Entities.emplace_back(e);
	}

	const std::vector<size_t>& EntityManager::getEntities() const
	{
		return m_Entities;
	}

	const std::string& EntityManager::getTag(size_t e) const
	{
		return EntityMemoryPool::Get().getTag(e);
	}

	void EntityManager::setSelectedEntity(size_t entity)
	{
		m_SelectedEntity = entity;
	}

	const size_t EntityManager::getSelectedEntity() const
	{
		return m_SelectedEntity;
	}

}
