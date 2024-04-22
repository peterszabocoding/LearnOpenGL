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

		template<typename T>
		std::vector<T>& getComponents() {
			return EntityMemoryPool::Get().getComponents<T>();
		}

		const std::vector<size_t>& getEntities() const;

		const std::string& getTag(size_t e) const;
		void setTag(size_t entityID, const std::string& newTag);

		void setSelectedEntity(size_t entity);

		const size_t getSelectedEntity() const;

	private:
		EntityManager() {};

	private:
		size_t m_SelectedEntity = -1;
		std::vector<size_t> m_Entities;

	};

}