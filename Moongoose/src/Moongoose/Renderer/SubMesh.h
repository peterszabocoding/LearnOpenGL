#pragma once

#include <string>
#include <vector>

#include "Moongoose/Core.h"
#include "Moongoose/Renderer/Buffer.h"
#include "Moongoose/Renderer/VertexArray.h"

namespace Moongoose {

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

