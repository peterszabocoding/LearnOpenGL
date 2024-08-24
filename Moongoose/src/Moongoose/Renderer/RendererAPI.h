#pragma once

#include "glm/glm.hpp"
#include "VertexArray.h"

namespace Moongoose
{
	class RendererAPI
	{
	public:
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
		void SetClearColor(const glm::vec4& color);
		void Clear();
		void DrawIndexed(const Ref<VertexArray>& vertexArray);
		void DrawIndexedStrip(const Ref<VertexArray>& vertexArray);
		void DrawShadowMap(const Ref<VertexArray>& vertexArray);
	};
}
