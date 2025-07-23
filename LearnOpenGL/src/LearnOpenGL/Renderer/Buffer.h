#pragma once
#include <stdint.h>
#include <string>
#include <vector>
#include "LearnOpenGL/Log.h"
#include "LearnOpenGL/Core.h"

namespace LearnOpenGL {

	enum class ShaderDataType {
		None = 0,
		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type) {
		switch (type) {
		case ShaderDataType::Float:			return 4;
		case ShaderDataType::Float2:		return 4 * 2;
		case ShaderDataType::Float3:		return 4 * 3;
		case ShaderDataType::Float4:		return 4 * 4;
		case ShaderDataType::Mat3:			return 4 * 3 * 3;
		case ShaderDataType::Mat4:			return 4 * 4 * 4;
		case ShaderDataType::Int:			return 4;
		case ShaderDataType::Int2:			return 4 * 2;
		case ShaderDataType::Int3:			return 4 * 3;
		case ShaderDataType::Int4:			return 4 * 4;
		case ShaderDataType::Bool:			return 1;
		}

		MG_ASSERT(true, "Unknown shader data type");
		return 0;
	}

	struct BufferElement {
		std::string Name;
		ShaderDataType Type;
		uint32_t Offset;
		uint32_t Size;
		bool Normalized;

		BufferElement() {}

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false) : Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {}

		uint32_t GetComponentCount() const {
			switch (Type) {
			case ShaderDataType::Float:			return 1;
			case ShaderDataType::Float2:		return 2;
			case ShaderDataType::Float3:		return 3;
			case ShaderDataType::Float4:		return 4;
			case ShaderDataType::Int:			return 1;
			case ShaderDataType::Int2:			return 2;
			case ShaderDataType::Int3:			return 3;
			case ShaderDataType::Int4:			return 4;
			case ShaderDataType::Mat3:			return 3 * 3;
			case ShaderDataType::Mat4:			return 4 * 4;
			case ShaderDataType::Bool:			return 1;
			}
			MG_ASSERT(true, "Unknown shader data type");
			return 0;
		}

	};

	class BufferLayout {
	public:
		BufferLayout() {}

		BufferLayout(const std::initializer_list<BufferElement>& elements) : m_Elements(elements) {
			CalculateOffsetsAndStride();
		}

		inline const std::vector<BufferElement>& GetElements() const { return m_Elements; }
		inline const uint32_t GetStride() const { return m_Stride; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }

		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetsAndStride() {
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements) {
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}

	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer {
	public:
		VertexBuffer(float* vertices, uint32_t size);
		~VertexBuffer();

		void Bind() const;
		void Unbind() const;

		void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
		const BufferLayout& GetLayout() const { return m_Layout; }

		static VertexBuffer* Create(float* vertices, uint32_t size);
	private:
		uint32_t m_VertexBufferID;
		BufferLayout m_Layout;
	};

	class IndexBuffer {
	public:
		IndexBuffer(uint32_t* indices, uint32_t count);
		~IndexBuffer();

		void Bind() const;
		void Unbind() const;

		uint32_t GetCount() const { return m_Count; }

		static IndexBuffer* Create(uint32_t* indices, uint32_t count);
	private:
		uint32_t m_IndexBufferID;
		uint32_t m_Count;
	};

	struct Buffer
	{
		void* Data;
		uint32_t Size;

		Buffer()
			: Data(nullptr), Size(0)
		{
		}

		Buffer(void* data, uint32_t size)
			: Data(data), Size(size)
		{
		}

		static Buffer copy(const void* data, uint32_t size)
		{
			Buffer buffer;
			buffer.allocate(size);
			memcpy(buffer.Data, data, size);
			return buffer;
		}

		void allocate(uint32_t size)
		{
			delete[](uint8_t*)Data;
			Data = nullptr;

			if (size == 0)
				return;

			Data = new uint8_t[size];
			Size = size;
		}

		void release()
		{
			delete[](uint8_t*)Data;
			Data = nullptr;
			Size = 0;
		}

		void zeroInitialize()
		{
			if (Data)
				memset(Data, 0, Size);
		}

		template<typename T>
		T& read(uint32_t offset = 0)
		{
			return *(T*)((byte*)Data + offset);
		}

		uint8_t* readBytes(uint32_t size, uint32_t offset)
		{
			MG_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
			uint8_t* buffer = new uint8_t[size];
			memcpy(buffer, (uint8_t*)Data + offset, size);
			return buffer;
		}

		void write(const void* data, uint32_t size, uint32_t offset = 0)
		{
			MG_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
			memcpy((uint8_t*)Data + offset, data, size);
		}

		operator bool() const
		{
			return Data;
		}

		uint8_t& operator[](int index)
		{
			return ((uint8_t*)Data)[index];
		}

		uint8_t operator[](int index) const
		{
			return ((uint8_t*)Data)[index];
		}

		template<typename T>
		T* As() const
		{
			return (T*)Data;
		}

		inline uint32_t getSize() const { return Size; }
	};
}