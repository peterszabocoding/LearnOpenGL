#pragma once

#include <vector>
#include <string>
#include "Entity.h"
#include "EntityMemoryPool.h"

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
		T& addComponent(size_t entity)
		{
			return m_Pool.addComponent<T>(entity);
		}

		template<typename T>
		bool hasComponent(size_t entity) {
			return m_Pool.hasComponent<T>(entity);
		}

		template<typename T>
		T& getComponent(size_t entity) {
			return m_Pool.getComponent<T>(entity);
		}

		template<typename T>
		std::vector<T>& getComponents() {
			return m_Pool.getComponents<T>();
		}

		std::vector<size_t> getEntities() const;

		const std::string& getTag(size_t e) const;

		void setTag(size_t entityID, const std::string& newTag);

		void setSelectedEntity(size_t entity);

		const size_t getSelectedEntity() const;

	private:
		EntityManager() {};

	private:
		size_t m_SelectedEntity = -1;
		std::vector<size_t> m_Entities;
		EntityMemoryPool m_Pool;
	};

}