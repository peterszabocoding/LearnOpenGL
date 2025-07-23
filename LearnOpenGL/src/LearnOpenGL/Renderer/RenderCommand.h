#pragma once

#include "LearnOpenGL/Core.h"
#include "RendererAPI.h"
#include "Material.h"

namespace LearnOpenGL
{
	struct MeshCommand
	{
		size_t id = -1;
		glm::mat4 transform = glm::mat4(1.0f);

		Ref<VertexArray> vertexArray;
		Ref<Material> material;
	};

	struct BillboardCommand
	{
		size_t id = -1;
		glm::mat4 transform = glm::mat4(1.0f);
		Ref<Texture2D> texture;
		float scale = 0.75f;
		glm::vec3 tintColor = glm::vec3(1.0f, 1.0f, 1.0f);
	};

	class RenderCommand
	{
	public:
		static void SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		static void Clear()
		{
			s_RendererAPI->Clear();
		}

		static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			vertexArray->Bind();
			s_RendererAPI->DrawIndexed(vertexArray);
			vertexArray->Unbind();
			drawCount++;
		}

		static void DrawIndexedStrip(const Ref<VertexArray>& vertexArray)
		{
			vertexArray->Bind();
			s_RendererAPI->DrawIndexedStrip(vertexArray);
			vertexArray->Unbind();
			drawCount++;
		}

		static unsigned int GetDrawCallCount() { return drawCount; }

		static void ResetCounter()
		{
			drawCount = 0;
		}

	private:
		static Scope<RendererAPI> s_RendererAPI;
		static unsigned int drawCount;
	};
}
