#pragma once
#include "EntityManager.h"
#include "EntityMemoryPool.h"
#include "Components.h"

namespace Moongoose {

	class World
	{

	public:
		const UUID& GetID() const { return m_ID; }
		const std::string& GetName() const { return m_Name; }

		Entity CreateEntity(const std::string& tag)
		{
			bool isTagReserved = m_EntityPool.isTagReserved(tag);
			Entity e = m_EntityPool.addEntity(tag);

			if (isTagReserved)
			{
				m_EntityPool.setTag(e, tag + std::to_string(e));
			}

			m_EntityPool.addComponent<TransformComponent>(e);
			return e;

		}

		template<typename T>
		T& AddComponent(size_t entity)
		{
			return m_EntityPool.addComponent<T>(entity);
		}

		template<typename T>
		bool HasComponent(size_t entity) 
		{
			return m_EntityPool.hasComponent<T>(entity);
		}

		template<typename T>
		T& GetComponent(size_t entity) 
		{
			return m_EntityPool.getComponent<T>(entity);
		}

		template<typename T>
		std::vector<T>& GetComponents() 
		{
			return m_EntityPool.getComponents<T>();
		}

		std::vector<size_t> GetEntities() const; 

		void SetTag(size_t entityID, const std::string& newTag);
		const std::string& GetTag(size_t entityID) const;

		void SetSelectedEntity(size_t entity) 
		{
			m_SelectedEntity = entity;
		}

		const size_t GetSelectedEntity() const { return m_SelectedEntity; }

	public:
		UUID m_ID;
		std::string m_Name;
		EntityMemoryPool m_EntityPool;
		Entity m_SelectedEntity = -1;
	};

}
