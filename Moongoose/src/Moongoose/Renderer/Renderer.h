#pragma once

#include "Moongoose/Core.h"
#include "VertexArray.h"
#include <glm/glm.hpp>

#include "PerspectiveCamera.h"

namespace Moongoose {
	class Framebuffer;
	class World;

	class Renderer {

	public:
		static void SetResolution(glm::uvec2 resolution);
		static void RenderWorld(const Ref<PerspectiveCamera>& camera, const Ref<World>& world);

		static Ref<Framebuffer> GetRenderBuffer() { return m_RenderBuffer; }

		static void BeginScene();
		static void EndScene();
		static void RenderMesh(const Ref<VertexArray>& vertexArray);

	private:

		static Ref<Framebuffer> m_RenderBuffer;
		static glm::uvec2 m_Resolution;
	};

}
