#pragma once

#include "Moongoose/Core.h"
#include "RendererAPI.h"
#include "Material.h"

namespace Moongoose {

	struct MeshCommand
	{
		MeshCommand(const Ref<VertexArray>& vArray, const Ref<Material>& mat) : vertexArray(vArray), material(mat) {}
		MeshCommand(const size_t id, const glm::mat4& transform, const Ref<VertexArray>& vArray, const Ref<Material>& mat) :
			id(id),
			transform(transform),
			vertexArray(vArray),
			material(mat) {}

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

	class RenderCommand {
	public:

		inline static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		inline static void SetClearColor(const glm::vec4& color) {
			s_RendererAPI->SetClearColor(color);
		}

		inline static void Clear() {
			s_RendererAPI->Clear();
		}

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray) {
			vertexArray->Bind();
			s_RendererAPI->DrawIndexed(vertexArray);
			vertexArray->Unbind();
		}

	private:
		static Scope<RendererAPI> s_RendererAPI;
	};

}