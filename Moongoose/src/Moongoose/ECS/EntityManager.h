#pragma once

#include <string>
#include <vector>
#include <tuple>
#include <queue>
#include "Moongoose/Core.h"
#include "Entity.h"
#include "Components.h"

namespace Moongoose
{
	class EntityManager
	{
	public:
		EntityManager();
		~EntityManager() = default;

		Entity CreateEntity();

		void DestroyEntity(Entity entity);

		void SetSignature(Entity entity, Signature signature);

		Signature GetSignature(Entity entity);

		uint32_t GetLivingEntityCount() const { return m_LivingEntityCount; }

	private:
		std::deque<Entity> m_AvailableEntities{};
		uint32_t m_LivingEntityCount{};
		std::array<Signature, MAX_ENTITIES> m_Signatures{};
	};
}
