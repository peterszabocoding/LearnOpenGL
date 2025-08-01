#pragma once

#include "Entity.h"
#include "Components.h"
#include "EntityManager.h"
#include "SystemManager.h"
#include "ComponentManager.h"

namespace LearnOpenGL
{
	class World
	{
	public:
		void Init();

		const UUID& GetID() const { return m_ID; }
		const std::string& GetName() const { return m_Name; }

		// Entites
		Entity CreateEntity(const std::string& tag) const;
		void DestroyEntity(Entity entity);

		uint32_t GetEntityCount() const
		{
			return m_EntityManager->GetLivingEntityCount();
		}

		void SetSelectedEntity(Entity entity)
		{
			m_SelectedEntity = entity;
		}

		const Entity GetSelectedEntity() const { return m_SelectedEntity; }

		// Components
		template <typename T>
		void RegisterComponent()
		{
			m_ComponentManager->RegisterComponent<T>();
		}

		template <typename T>
		void AddComponent(Entity entity, T component)
		{
			m_ComponentManager->AddComponent<T>(entity, component);

			Signature signature = m_EntityManager->GetSignature(entity);
			signature.set(m_ComponentManager->GetComponentType<T>(), true);
			m_EntityManager->SetSignature(entity, signature);

			m_SystemManager->EntitySignatureChanged(entity, signature);
		}

		template <typename T>
		bool HasComponent(Entity entity)
		{
			return m_ComponentManager->HasComponent<T>(entity);
		}

		template <typename T>
		T& GetComponent(Entity entity)
		{
			return m_ComponentManager->GetComponent<T>(entity);
		}

		template <typename T>
		std::vector<T> GetComponentsByType()
		{
			return m_ComponentManager->GetComponentsByType<T>();
		}

		template <typename T>
		ComponentType GetComponentType() const
		{
			return m_ComponentManager->GetComponentType<T>();
		}

		template <typename T>
		Ref<ComponentArray<T>> GetComponentArray() const
		{
			return m_ComponentManager->GetComponentArray<T>();
		}

		//Systems
		template <typename T>
		Ref<T> RegisterSystem()
		{
			Ref<T> system = m_SystemManager->RegisterSystem<T>();
			SetSystemSignature<T>((std::static_pointer_cast<System>(system))->GetSystemSignature(this));
			return system;
		}

		template <typename T>
		Ref<T> GetSystem()
		{
			return m_SystemManager->GetSystem<T>();
		}

		template <typename T>
		void SetSystemSignature(Signature signature)
		{
			m_SystemManager->SetSignature<T>(signature);
		}

	public:
		UUID m_ID;
		std::string m_Name;
		Entity m_SelectedEntity = -1;

		Scope<EntityManager> m_EntityManager;
		Scope<SystemManager> m_SystemManager;
		Scope<ComponentManager> m_ComponentManager;
	};
}
