#pragma once

#include "Scene.h"
#include "Moongoose/Renderer/PerspectiveCamera.h"

namespace Moongoose {

	static Ref<PerspectiveCamera> m_RenderCamera;

	class RenderSystem
	{
	public:
		static void SetCamera(Ref<PerspectiveCamera> camera);

		static Ref<PerspectiveCamera> GetCamera();

		static void Run();

	private:
		static glm::mat4 getModelMatrix(const TransformComponent& component);

		
	};
}

