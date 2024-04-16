#include "mgpch.h"
#include "Scene.h"

namespace Moongoose {

	Entity Scene::AddEntity(std::string name)
	{
		//Ref<Entity> e = CreateRef<Entity>(name);
		//m_Entities.push_back(e);
		//return e;
		return 0;
	}

	std::vector<Entity>& Scene::getEntities()
	{
		return m_Entities;
	}

}
