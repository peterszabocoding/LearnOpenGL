#pragma once
#include <glad/glad.h>
#include <string>
#include <vector>

#include "Moongoose/Core.h"
#include "Moongoose/Asset/Asset.h"
#include "Moongoose/Renderer/Buffer.h"
#include "Moongoose/Renderer/VertexArray.h"
#include "Moongoose/Renderer/Bounds.h"
#include "Moongoose/Renderer/Material.h"
#include "Moongoose/Renderer/SubMesh.h"

namespace Moongoose {

	class Mesh: public Asset
	{

	public:
		Mesh() {}
		~Mesh() = default;

		virtual AssetType getAssetType() const override { return AssetType::Mesh; }
		static AssetType GetStaticAssetType() { return AssetType::Mesh; }

		const Ref<SubMesh>& AddSubmesh(
			unsigned int materialIndex,
			float* vertices,
			unsigned int* indices,
			unsigned int numOfVertices,
			unsigned int numOfIndices)
		{
			return AddSubmesh(materialIndex, vertices, indices, numOfVertices, numOfIndices, {
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float2, "aTexCoords" },
				{ ShaderDataType::Float3, "aNormal" },
				{ ShaderDataType::Float3, "aTangent" },
				{ ShaderDataType::Float3, "aBitangent" }
				});
		}

		const Ref<SubMesh>& AddSubmesh(
			unsigned int materialIndex,
			float* vertices, 
			unsigned int* indices, 
			unsigned int numOfVertices, 
			unsigned int numOfIndices, 
			BufferLayout bufferLayout);

		void SetModelSource(const std::string& source) { this->modelSource = source; }
		const std::string& GetModelSource() const { return modelSource; };

		const std::vector<Ref<SubMesh>>& GetSubmeshes() const { return m_Submeshes; }

		void AddMaterial(const Ref<Material> material) { m_Materials.push_back(material); }
		void SetMaterial(unsigned int index, const Ref<Material>& material) { m_Materials[index] = material; }

		const std::vector<Ref<Material>>& GetMaterials() const { return m_Materials; }
		Ref<Material> GetMaterial(unsigned int index) const { return m_Materials[index]; }

		const Bounds3& GetBounds() const;
		void SetBounds(const Bounds3& bounds);

	private:
		std::string modelSource = "";
		std::vector<Ref<SubMesh>> m_Submeshes;
		std::vector<Ref<Material>> m_Materials;
		Bounds3 m_Bounds;
	};


}
