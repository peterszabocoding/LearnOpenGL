#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>

#include "Moongoose/Core.h"
#include "Moongoose/Renderer/Buffer.h"
#include "Moongoose/Renderer/VertexArray.h"

namespace Moongoose {

	class Mesh
	{

	public:
		Mesh() {}
		Mesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices);
		Mesh(GLfloat* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices, BufferLayout bufferLayout);
		~Mesh() = default;

		void SetMaterialID(const std::string& materialId) { this->materialID = materialId; }
		const std::string& GetMaterialID() const { return materialID; };
		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }

	private:
		std::string materialID;
		Ref<VertexArray> m_VertexArray;
	};


}
