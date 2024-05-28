#pragma once

#include "WorldManager.h"
#include "SystemManager.h"
#include "Moongoose/Renderer/PerspectiveCamera.h"

namespace Moongoose {

	class RenderSystem: public System
	{
	public:
		virtual Signature GetSystemSignature(World* world);

		void Run(const Ref<PerspectiveCamera>& camera, Ref<World> world);

	};
}

