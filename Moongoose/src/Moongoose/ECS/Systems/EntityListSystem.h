#pragma once

#include <string>
#include <vector>

#include "Moongoose/ECS/SystemManager.h"

namespace Moongoose
{
	class EntityListSystem: public System
	{
	public:
		virtual Signature EntityListSystem::GetSystemSignature(Moongoose::World* world);

		const std::vector<std::string> GetEntityTagList(Ref<World> world) const;

	};
}

