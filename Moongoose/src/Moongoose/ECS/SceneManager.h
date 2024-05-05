#pragma once

#include "Moongoose/Core.h"
#include "Entity.h"
#include "EntityManager.h"

namespace Moongoose {

	class SceneManager
	{
	public:
		static SceneManager& Get()
		{
			static SceneManager manager;
			return manager;
		}

		Entity AddEntity(const std::string& name);
		void RemoveEntity(const Entity& entity);

		const std::vector<Entity>& getEntities() const;
	private:
		SceneManager();
		~SceneManager() = default;

	private:
		std::vector<Entity> m_Entities;
		EntityManager* m_EntityManager;
	};

}

