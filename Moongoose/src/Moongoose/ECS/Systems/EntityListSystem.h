#pragma once

#include <string>
#include <vector>

#include "Moongoose/ECS/SystemManager.h"

namespace Moongoose
{
	class EntityListSystem: public System
	{
	public:
		virtual Signature EntityListSystem::GetSystemSignature(World* world) override;

		std::vector<std::string> GetEntityTagList(const Ref<World>& world) const;

	};
}

