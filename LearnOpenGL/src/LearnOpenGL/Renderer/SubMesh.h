#pragma once

#include <string>
#include <vector>

#include "LearnOpenGL/Core.h"
#include "LearnOpenGL/Renderer/Buffer.h"
#include "LearnOpenGL/Renderer/VertexArray.h"

namespace LearnOpenGL {

	class SubMesh
	{
	public:
		SubMesh(unsigned int matIndex, const Ref<VertexArray>& vArray) : materialIndex(matIndex), vertexArray(vArray) {}
		~SubMesh() = default;
	public:
		unsigned int materialIndex = 0;
		Ref<VertexArray> vertexArray;
	};

}

