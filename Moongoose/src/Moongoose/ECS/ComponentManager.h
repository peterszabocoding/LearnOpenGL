#pragma once

#include <unordered_map>
#include "Components.h"
#include "ComponentArray.h"
#include "Moongoose/Core.h"

namespace Moongoose
{
	class ComponentManager
	{
	public:
		template <typename T>
		void RegisterComponent()
		{
			const char* typeName = typeid(T).name();
			MG_ASSERT(m_ComponentTypes.find(typeName) == m_ComponentTypes.end(),
			          "Registering component type more than once.");

			m_ComponentTypes.insert({typeName, m_NextComponentType});
			m_ComponentArrays.insert({typeName, CreateRef<ComponentArray<T>>()});

			++m_NextComponentType;
		}

		template <typename T>
		ComponentType GetComponentType()
		{
			const char* typeName = typeid(T).name();
			MG_ASSERT(m_ComponentTypes.find(typeName) != m_ComponentTypes.end(),
			          "Component not registered before use.");

			return m_ComponentTypes[typeName];
		}

		template <typename T>
		void AddComponent(Entity entity, T component)
		{
			GetComponentArray<T>()->InsertData(entity, component);
		}

		template <typename T>
		void RemoveComponent(Entity entity)
		{
			GetComponentArray<T>()->RemoveData(entity);
		}

		template <typename T>
		T& GetComponent(Entity entity)
		{
			return GetComponentArray<T>()->GetData(entity);
		}

		template <typename T>
		bool HasComponent(Entity entity)
		{
			return GetComponentArray<T>()->HasData(entity);
		}

		template <typename T>
		std::vector<T> GetComponentsByType()
		{
			return GetComponentArray<T>()->GetComponents();
		}

		void EntityDestroyed(Entity entity)
		{
			for (auto const& pair : m_ComponentArrays)
			{
				auto const& component = pair.second;
				component->EntityDestroyed(entity);
			}
		}

	public:
		template <typename T>
		Ref<ComponentArray<T>> GetComponentArray()
		{
			const char* typeName = typeid(T).name();
			assert(m_ComponentTypes.find(typeName) != m_ComponentTypes.end() && "Component not registered before use.");
			return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArrays[typeName]);
		}

	private:
		std::unordered_map<const char*, ComponentType> m_ComponentTypes{};
		std::unordered_map<const char*, Ref<IComponentArray>> m_ComponentArrays{};
		ComponentType m_NextComponentType{};
	};
}
