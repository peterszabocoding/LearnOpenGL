#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>

#include "Moongoose/Core.h"
#include "Moongoose/Asset/Asset.h"
#include "Moongoose/Renderer/Buffer.h"
#include "Moongoose/Renderer/VertexArray.h"
#include "Moongoose/Renderer/Bounds.h"

namespace Moongoose {

	class Mesh: public Asset
	{

	public:
		Mesh() {}
		Mesh(float* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices);
		Mesh(float* vertices, unsigned int* indices, unsigned int numOfVertices, unsigned int numOfIndices, BufferLayout bufferLayout);
		~Mesh() = default;

		virtual AssetType getAssetType() const override { return AssetType::Mesh; }
		static AssetType GetStaticAssetType() { return AssetType::Mesh; }

		void SetMaterialID(const std::string& materialId) { this->materialID = materialId; }
		void SetModelSource(const std::string& source) { this->modelSource = source; }
		void SetBounds(const Bounds3& bounds);

		const std::string& GetMaterialID() const { return materialID; };
		const std::string& GetModelSource() const { return modelSource; };
		const Ref<VertexArray>& GetVertexArray() const { return m_VertexArray; }
		const Bounds3& GetBounds() const;

	private:
		std::string materialID;
		std::string modelSource = "";
		Ref<VertexArray> m_VertexArray;
		Bounds3 m_Bounds;
	};


}
