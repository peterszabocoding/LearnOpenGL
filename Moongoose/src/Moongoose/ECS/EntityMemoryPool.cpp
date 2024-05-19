#include "mgpch.h"
#include "EntityMemoryPool.h"

namespace Moongoose {
	
	EntityMemoryPool::EntityMemoryPool() {
		std::vector<TransformComponent>& transformComponentList = std::get<std::vector<TransformComponent>>(m_Pool);
		std::vector<MeshComponent>& meshComponentList = std::get<std::vector<MeshComponent>>(m_Pool);
		std::vector<LightComponent>& lightComponentList = std::get<std::vector<LightComponent>>(m_Pool);

		transformComponentList.reserve(MAX_ENTITIES);
		meshComponentList.reserve(MAX_ENTITIES);

		m_Tags.reserve(MAX_ENTITIES);
		m_Active.reserve(MAX_ENTITIES);

		for (size_t i = 0; i < MAX_ENTITIES; i++)
		{
			transformComponentList.push_back(TransformComponent());
			meshComponentList.push_back(MeshComponent());
			lightComponentList.push_back(LightComponent());
			m_Tags.push_back("");
			m_Active.push_back(false);
		}
	}

	size_t EntityMemoryPool::getNextEntityIndex()
	{
		if (m_Active.size() == 0) return 0;

		for (size_t i = 0; i < m_Active.size(); i++)
		{
			if (!m_Active[i]) return i;
		}

		MG_CORE_ASSERT(false, "Ran out of entities");
		return 0;
	}

	size_t EntityMemoryPool::addEntity(std::string tag)
	{
		size_t e = getNextEntityIndex();
		m_Tags[e] = tag;
		m_Active[e] = true;
		return e;
	}

	const std::string& EntityMemoryPool::getTag(size_t entityID) const
	{
		return m_Tags[entityID];
	}

	void EntityMemoryPool::setTag(size_t entityID, const std::string& newTag)
	{
		m_Tags[entityID] = newTag;
	}

	bool EntityMemoryPool::isTagReserved(const std::string& tag) const
	{
		for (auto& t : m_Tags) if (t == tag) return true;
		return false;
	}

	bool EntityMemoryPool::isEntityActive(size_t entityID) const
	{
		if (m_Active.size() <= entityID) return false;
		return m_Active[entityID];
	}

}