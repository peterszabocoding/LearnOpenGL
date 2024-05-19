#include "mgpch.h"
#include "EntityManager.h"

#include "EntityMemoryPool.h"

namespace Moongoose {

	Entity EntityManager::addEntity(std::string tag)
	{
		bool isTagReserved = m_Pool.isTagReserved(tag);

		Entity e = m_Pool.addEntity(tag);

		if (isTagReserved)
		{
			m_Pool.setTag(e, tag + std::to_string(e));
		}

		m_Pool.addComponent<TransformComponent>(e);
		return m_Entities.emplace_back(e);
	}

	std::vector<size_t> EntityManager::getEntities() const
	{
		std::vector<Entity> m_ActiveEntities;
		for (auto& e : m_Entities)
		{
			if (m_Pool.isEntityActive(e))
				m_ActiveEntities.push_back(e);
		}
		return m_ActiveEntities;
	}

	const std::string& EntityManager::getTag(size_t e) const
	{
		return m_Pool.getTag(e);
	}

	void EntityManager::setTag(size_t e, const std::string& newTag)
	{
		return m_Pool.setTag(e, newTag);
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
