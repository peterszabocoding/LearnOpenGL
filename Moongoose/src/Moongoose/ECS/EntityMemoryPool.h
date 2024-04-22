#pragma once

#include "Moongoose/Core.h"
#include "Entity.h"
#include "Components.h"
#include <string>
#include <vector>
#include <tuple>

namespace Moongoose {

	constexpr unsigned int MAX_ENTITIES = 10000;

	typedef std::tuple<
		std::vector<TransformComponent>,
		std::vector<MeshComponent>,
		std::vector<LightComponent>> EntityComponentVectorTuple;

	class EntityMemoryPool
	{
	public:

		static EntityMemoryPool& Get()
		{
			static EntityMemoryPool pool(MAX_ENTITIES);
			return pool;
		}

		template<typename T>
		T& addComponent(size_t entityID)
		{
			((Component*) &std::get<std::vector<T>>(m_Pool)[entityID])->m_Active = true;
			return getComponent<T>(entityID);
		}
		
		template<typename T>
		T& getComponent(size_t entityID)
		{
			return std::get<std::vector<T>>(m_Pool)[entityID];
		}

		template<typename T>
		std::vector<T>& getComponents()
		{
			return std::get<std::vector<T>>(m_Pool);
		}

		template<typename T>
		bool hasComponent(size_t entityID)
		{
			return static_cast<Component>(getComponent<T>(entityID)).m_Active;
		}

		size_t addEntity(std::string tag);

		const std::string& getTag(size_t entityID) const;
		void setTag(size_t entityID, const std::string& newTag);
		bool isTagReserved(const std::string& tag) const;

	private:
		EntityMemoryPool(size_t maxEntities);

		size_t getNextEntityIndex();

	private:
		EntityComponentVectorTuple m_Pool;
		std::vector<std::string> m_Tags;
		std::vector<bool> m_Active;
	};

}