#include "mgpch.h"
#include "World.h"

namespace Moongoose {
	void World::Init()
	{
		m_ComponentManager = CreateScope<ComponentManager>();
		m_EntityManager = CreateScope<EntityManager>();
		m_SystemManager = CreateScope<SystemManager>();
	}
}
