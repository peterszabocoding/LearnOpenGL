#include "mgpch.h"
#include "Mesh.h"

namespace Moongoose {

	Mesh::Mesh(float* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices) : Mesh(vertices, indices, numOfVertices, numOfIndices, {
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float2, "aTexCoords" },
				{ ShaderDataType::Float3, "aNormal" },
				{ ShaderDataType::Float3, "aTangent" },
				{ ShaderDataType::Float3, "aBitangent" }
		}) {}

	Mesh::Mesh(float* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices, BufferLayout bufferLayout)
	{
		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;

		m_VertexArray.reset(VertexArray::Create());
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices[0]) * numOfVertices));

		vertexBuffer->SetLayout(bufferLayout);

		m_VertexArray->AddVertexBuffer(vertexBuffer);

		indexBuffer.reset(IndexBuffer::Create(indices, numOfIndices));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		m_VertexArray->Unbind();
	}

	void Mesh::SetBounds(const Bounds3& bounds)
	{
		m_Bounds = bounds;
	}

	const Bounds3& Mesh::GetBounds() const
	{
		return m_Bounds;
	}

}