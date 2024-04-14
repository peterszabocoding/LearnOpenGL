#include "mgpch.h"
#include "Scene.h"

namespace Moongoose {

	Ref<Entity> Scene::AddEntity(std::string name)
	{
		Ref<Entity> e = CreateRef<Entity>(name);
		m_Entities.push_back(e);
		return e;
	}

	std::vector<Ref<Entity>>& Scene::getEntities()
	{
		return m_Entities;
	}

}
