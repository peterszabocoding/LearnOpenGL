#pragma once

#include "Moongoose/Core.h"
#include "VertexArray.h"
#include <glm/glm.hpp>

namespace Moongoose {

	class Renderer {

	public:
		static void BeginScene();
		static void EndScene();

		static void RenderMesh(const Ref<VertexArray>& vertexArray);
		//static void RenderMesh(Shader* shader, Material* material, const Ref<VertexArray>& vertexArray, const glm::mat4& modelMatrix);
	};

}