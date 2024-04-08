#include "mgpch.h"

#include <glad/glad.h>
#include "Moongoose/Log.h"
#include "VertexArray.h"

namespace Moongoose {

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float:     return GL_FLOAT;
		case ShaderDataType::Float2:    return GL_FLOAT;
		case ShaderDataType::Float3:    return GL_FLOAT;
		case ShaderDataType::Float4:    return GL_FLOAT;
		case ShaderDataType::Int:       return GL_INT;
		case ShaderDataType::Int2:      return GL_INT;
		case ShaderDataType::Int3:      return GL_INT;
		case ShaderDataType::Int4:      return GL_INT;
		case ShaderDataType::Mat3:      return GL_FLOAT;
		case ShaderDataType::Mat4:      return GL_FLOAT;
		case ShaderDataType::Bool:      return GL_BOOL;
		}
		MG_ASSERT(true, "Unknown shader data type");
		return 0;
	}

	VertexArray::VertexArray()
	{
		glCreateVertexArrays(1, &m_VertexArrayID);
	}

	VertexArray::~VertexArray()
	{
		glDeleteVertexArrays(1, &m_VertexArrayID);
	}

	void VertexArray::Bind() const
	{
		glBindVertexArray(m_VertexArrayID);
	}

	void VertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		glBindVertexArray(m_VertexArrayID);
		vertexBuffer->Bind();

		uint32_t index = 0;
		const auto& layout = vertexBuffer->GetLayout();

		MG_ASSERT(true, layout.GetElements().size());

		for (const auto& element : layout) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.Type),
				element.Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(void*)element.Offset);
			index++;
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(m_VertexArrayID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

	VertexArray* VertexArray::Create()
	{
		return new VertexArray();
	}

}