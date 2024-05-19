#include "mgpch.h"
#include "World.h"

namespace Moongoose {

	std::vector<size_t> World::GetEntities() const
	{
		std::vector<Entity> activeEntites;
		auto& actives = m_EntityPool.GetActiveList();

		for (size_t i = 0; i < actives.size(); i++)
		{
			if (actives[i]) 
			{
				activeEntites.push_back(i);
				continue;
			}
			break;
		}
		return activeEntites;
	}

	void World::SetTag(size_t entityID, const std::string& newTag)
	{
		return m_EntityPool.setTag(entityID, newTag);
	}

	const std::string& World::GetTag(size_t entityID) const
	{
		return m_EntityPool.getTag(entityID);
	}
}
