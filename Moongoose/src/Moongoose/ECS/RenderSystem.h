#pragma once

#include "Scene.h"
#include "Moongoose/Renderer/PerspectiveCamera.h"

namespace Moongoose {

	class RenderSystem
	{
	public:
		static void Run(Ref<Scene> scene, Ref<PerspectiveCamera> camera);

	private:
		static glm::mat4 getModelMatrix(Ref<TransformComponent> component);
	};


}

