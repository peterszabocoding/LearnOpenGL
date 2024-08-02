#pragma once

#include <set>
#include <unordered_map>

#include "Entity.h"
#include "Moongoose/Core.h"
#include "Components.h"

namespace Moongoose {

	class World;

	class System
	{
	public:
		virtual ~System() = default;
		virtual Signature GetSystemSignature(World* world) = 0;
	public:
		std::set<Entity> m_Entities;
	};

	class SystemManager
	{
	public:

		template<typename T>
		Ref<T> RegisterSystem()
		{
			const char* typeName = typeid(T).name();

			MG_ASSERT(m_Systems.find(typeName) == m_Systems.end(), "Registering system more than once.");

			// Create a pointer to the system and return it so it can be used externally
			auto system = CreateRef<T>();
			m_Systems.insert({ typeName, system });
			return system;
		}

		template<typename T>
		Ref<T> GetSystem()
		{
			const char* typeName = typeid(T).name();
			MG_ASSERT(m_Systems.find(typeName) != m_Systems.end(), "System was not registered.");
			return std::static_pointer_cast<T>(m_Systems[typeName]);
		}

		template<typename T>
		void SetSignature(Signature signature)
		{
			const char* typeName = typeid(T).name();
			MG_ASSERT(m_Systems.find(typeName) != m_Systems.end(), "System used before registered.");
			m_Signatures.insert({ typeName, signature });
		}

		void EntityDestroyed(const Entity entity) const
		{
			for (const auto& [type, system] : m_Systems)
			{
				system->m_Entities.erase(entity);
			}
		}

		void EntitySignatureChanged(const Entity entity, const Signature entitySignature)
		{
			for (const auto& [type, system] : m_Systems)
			{
				if ((entitySignature & m_Signatures[type]) == m_Signatures[type])
					system->m_Entities.insert(entity);
				else
					system->m_Entities.erase(entity);
			}
		}

	private:
		std::unordered_map<const char*, Signature> m_Signatures{};
		std::unordered_map<const char*, Ref<System>> m_Systems{};
	};
}