#pragma once

#include "SceneManager.h"
#include "Moongoose/Renderer/PerspectiveCamera.h"

namespace Moongoose {

	class RenderSystem
	{
	public:
		static void Run(const Ref<PerspectiveCamera>& camera);

	};
}

