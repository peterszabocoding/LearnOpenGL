#pragma once

#include <unordered_map>
#include "Moongoose/Core.h"
#include "Entity.h"

namespace Moongoose
{
	class IComponentArray
	{
	public:
		virtual ~IComponentArray() = default;
		virtual void EntityDestroyed(Entity entity) = 0;
	};

	template <typename T>
	class ComponentArray : public IComponentArray
	{
	public:
		void InsertData(Entity entity, T component)
		{
			MG_ASSERT(m_EntityToIndexMap.find(entity) == m_EntityToIndexMap.end(),
			          "Component added to same entity more than once.");

			size_t newIndex = m_Size;
			m_EntityToIndexMap[entity] = newIndex;
			m_IndexToEntityMap[newIndex] = entity;
			m_ComponentArray[newIndex] = component;
			++m_Size;
		}

		void RemoveData(Entity entity)
		{
			MG_ASSERT(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end(), "Removing non-existent component.");

			// Copy element at end into deleted element's place to maintain density
			size_t indexOfRemovedEntity = m_EntityToIndexMap[entity];
			size_t indexOfLastElement = m_Size - 1;
			m_ComponentArray[indexOfRemovedEntity] = m_ComponentArray[indexOfLastElement];

			// Update map to point to moved spot
			Entity entityOfLastElement = m_IndexToEntityMap[indexOfLastElement];
			m_EntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
			m_IndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

			m_EntityToIndexMap.erase(entity);
			m_IndexToEntityMap.erase(indexOfLastElement);

			--m_Size;
		}

		T& GetData(Entity entity)
		{
			MG_ASSERT(m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end(),
			          "Retrieving non-existent component. " + std::to_string(entity))

			// Return a reference to the entity's component
			return m_ComponentArray[m_EntityToIndexMap[entity]];
		}

		bool HasData(Entity entity)
		{
			return m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end();
		}

		std::vector<T> GetComponents() const
		{
			std::vector<T> componentsVector(m_ComponentArray.begin(), m_ComponentArray.begin() + m_Size);
			return componentsVector;
		}

		void EntityDestroyed(Entity entity) override
		{
			if (m_EntityToIndexMap.find(entity) != m_EntityToIndexMap.end())
			{
				// Remove the entity's component if it existed
				RemoveData(entity);
			}
		}

	public:
		std::array<T, MAX_ENTITIES> m_ComponentArray;
		std::unordered_map<Entity, size_t> m_EntityToIndexMap;
		std::unordered_map<size_t, Entity> m_IndexToEntityMap;
		size_t m_Size;
	};
}
