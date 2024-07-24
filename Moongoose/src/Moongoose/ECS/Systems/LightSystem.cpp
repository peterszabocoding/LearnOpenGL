#include "mgpch.h"
#include "LightSystem.h"

namespace Moongoose
{
	Signature LightSystem::GetSystemSignature(World* world)
	{
		Signature signature;
		signature.set(world->GetComponentType<TransformComponent>());
		signature.set(world->GetComponentType<LightComponent>());

		return signature;
	}

	void LightSystem::Run(const Ref<PerspectiveCamera>& camera, Ref<World> world)
	{
	}
}
