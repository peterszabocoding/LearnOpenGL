#pragma once

#include <vector>
#include <string>
#include "Entity.h"

namespace Moongoose {

	class EntityManager
	{
	public:
		static EntityManager& Get()
		{
			static EntityManager manager;
			return manager;
		}

		Entity addEntity(std::string tag);

		template<typename T>
		bool hasComponent(size_t entity) {
			return EntityMemoryPool::Get().hasComponent<T>(entity);
		}

		template<typename T>
		T& getComponent(size_t entity) {
			return EntityMemoryPool::Get().getComponent<T>(entity);
		}

		const std::vector<Entity>& getEntities() const;

	private:
		EntityManager() {};

	private:
		std::vector<Entity> m_Entities;


	};

}