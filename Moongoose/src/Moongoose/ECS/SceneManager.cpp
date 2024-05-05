#include "mgpch.h"
#include "SceneManager.h"

namespace Moongoose {
	SceneManager::SceneManager() : m_EntityManager(&EntityManager::Get()) {}

	Entity SceneManager::AddEntity(const std::string& name)
	{
		return m_EntityManager->addEntity(name);
	}

	void SceneManager::RemoveEntity(const Entity& entity)
	{

	}

	const std::vector<Entity>& SceneManager::getEntities() const
	{
		return m_EntityManager->getEntities();
	}

}
