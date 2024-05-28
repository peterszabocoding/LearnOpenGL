#include "mgpch.h"

#include "Moongoose/ECS/Components.h"
#include "Moongoose/ECS/World.h"
#include "EntityListSystem.h"

namespace Moongoose
{
	Signature Moongoose::EntityListSystem::GetSystemSignature(World* world)
	{
		Signature signature;
		signature.set(world->GetComponentType<IDComponent>());
		signature.set(world->GetComponentType<TagComponent>());

		return signature;
	}
	const std::vector<std::string> EntityListSystem::GetEntityTagList(Ref<World> world) const
	{
		std::vector<std::string> tags;
		for (auto& e : m_Entities)
		{
			tags.push_back(world->GetComponent<TagComponent>(e).Tag);
		}
		return tags;
	}
}
