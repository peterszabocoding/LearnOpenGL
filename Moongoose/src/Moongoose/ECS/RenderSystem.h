#pragma once

#include "Scene.h"
#include "Moongoose/Renderer/PerspectiveCamera.h"

namespace Moongoose {

	class RenderSystem
	{
	public:
		static void Run(Ref<PerspectiveCamera> camera);

	private:
		static glm::mat4 getModelMatrix(const TransformComponent& component);
	};


}

