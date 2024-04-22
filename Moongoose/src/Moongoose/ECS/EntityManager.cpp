#include "mgpch.h"
#include "EntityManager.h"

#include "EntityMemoryPool.h"

namespace Moongoose {

	Entity EntityManager::addEntity(std::string tag)
	{
		bool isTagReserved = EntityMemoryPool::Get().isTagReserved(tag);

		Entity e = EntityMemoryPool::Get().addEntity(tag);

		if (isTagReserved)
		{
			EntityMemoryPool::Get().setTag(e, tag + std::to_string(e));
		}

		EntityMemoryPool::Get().addComponent<TransformComponent>(e);
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

	void EntityManager::setTag(size_t e, const std::string& newTag)
	{
		return EntityMemoryPool::Get().setTag(e, newTag);
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
