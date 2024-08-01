#include "mgpch.h"
#include "Mesh.h"

namespace Moongoose {

	void Mesh::SetBounds(const Bounds3& bounds)
	{
		m_Bounds = bounds;
	}

	const Ref<SubMesh>& Mesh::AddSubmesh(unsigned int materialIndex, float* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices, BufferLayout bufferLayout)
	{
		Ref<VertexArray> vertexArray;
		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;

		vertexArray.reset(VertexArray::Create());
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices[0]) * numOfVertices));
		indexBuffer.reset(IndexBuffer::Create(indices, numOfIndices));

		vertexBuffer->SetLayout(bufferLayout);
		vertexArray->AddVertexBuffer(vertexBuffer);
		vertexArray->SetIndexBuffer(indexBuffer);
		vertexArray->Unbind();

		Ref<SubMesh> submesh = CreateRef<SubMesh>(materialIndex, vertexArray);
		m_Submeshes.push_back(submesh);
		return submesh;
	}

	const Ref<SubMesh>& Mesh::AddSubmesh(unsigned int materialIndex, std::vector<VertexPTNTB>& vertices, std::vector<unsigned int>& indices)
	{
		float* verticesArray = new float[vertices.size() * sizeof(VertexPTNTB)];
		for (size_t i = 0; i < vertices.size(); i++)
		{
			for (size_t j = 0; j < sizeof(VertexPTNTB); j++)
			{
				verticesArray[i * sizeof(VertexPTNTB) + j] = vertices[i].ToFloatArray()[j];
			}
		}

		Ref<VertexArray> vertexArray;
		Ref<VertexBuffer> vertexBuffer;
		Ref<IndexBuffer> indexBuffer;

		vertexArray.reset(VertexArray::Create());
		vertexBuffer.reset(VertexBuffer::Create(verticesArray, sizeof(VertexPTNTB) * vertices.size()));
		indexBuffer.reset(IndexBuffer::Create(&indices[0], indices.size()));

		vertexBuffer->SetLayout(VertexPTNTB::GetBufferLayout());
		vertexArray->AddVertexBuffer(vertexBuffer);
		vertexArray->SetIndexBuffer(indexBuffer);
		vertexArray->Unbind();

		Ref<SubMesh> submesh = CreateRef<SubMesh>(materialIndex, vertexArray);
		m_Submeshes.push_back(submesh);
		return submesh;
	}

	const Bounds3& Mesh::GetBounds() const
	{
		return m_Bounds;
	}

}