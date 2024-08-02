#pragma once

#include "Moongoose/ECS/WorldManager.h"
#include "Moongoose/ECS/SystemManager.h"
#include "Moongoose/Renderer/PerspectiveCamera.h"

namespace Moongoose {

	class LightSystem : public System
	{
	public:
		LightSystem() = default;

		virtual Signature GetSystemSignature(World* world) override;

		void Run(const Ref<PerspectiveCamera>& camera, const Ref<World>& world) const;
	};
}

