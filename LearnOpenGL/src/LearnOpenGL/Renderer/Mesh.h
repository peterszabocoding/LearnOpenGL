#pragma once
#include <string>
#include <vector>

#include "LearnOpenGL/Core.h"
#include "LearnOpenGL/Asset/Asset.h"
#include "LearnOpenGL/Renderer/Buffer.h"
#include "LearnOpenGL/Renderer/Bounds.h"
#include "LearnOpenGL/Renderer/Material.h"
#include "LearnOpenGL/Renderer/SubMesh.h"

namespace LearnOpenGL {

	struct VertexP
	{
		glm::vec3 m_Position;

		float* ToFloatArray() {
			float arr[3] = { m_Position.x, m_Position.y, m_Position.z };
			return arr;
		}

		static BufferLayout GetBufferLayout() {
			return {
				{ ShaderDataType::Float3, "aPos" }
			};
		}
	};

	struct VertexPT
	{
		glm::vec3 m_Position;
		glm::vec2 m_TexCoord;

		float* ToFloatArray() {
			float arr[5] = { m_Position.x, m_Position.y, m_Position.z, m_TexCoord.x, m_TexCoord.y };
			return arr;
		}

		static BufferLayout GetBufferLayout() {
			return {
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float2, "aTexCoords" }
			};
		}
	};

	struct VertexPTN
	{
		glm::vec3 m_Position;
		glm::vec2 m_TexCoord;
		glm::vec3 m_Normal;

		float* ToFloatArray() {
			float arr[8] = { m_Position.x, m_Position.y, m_Position.z, m_TexCoord.x, m_TexCoord.y, m_Normal.x, m_Normal.y, m_Normal.z };
			return arr;
		}

		static BufferLayout GetBufferLayout() {
			return {
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float2, "aTexCoords" },
				{ ShaderDataType::Float3, "aNormal" }
			};
		}
	};

	struct VertexPTNTB
	{
		glm::vec3 m_Position;
		glm::vec2 m_TexCoord;
		glm::vec3 m_Normal;
		glm::vec3 m_Tangent;
		glm::vec3 m_Bitangent;

		float* ToFloatArray() {
			float arr[14] = { m_Position.x, m_Position.y, m_Position.z, m_TexCoord.x, m_TexCoord.y, m_Normal.x, m_Normal.y, m_Normal.z, m_Tangent.x, m_Tangent.y, m_Tangent.z, m_Bitangent.x, m_Bitangent.y, m_Bitangent.z };
			return arr;
		}

		static BufferLayout GetBufferLayout() {
			return {
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float2, "aTexCoords" },
				{ ShaderDataType::Float3, "aNormal" },
				{ ShaderDataType::Float3, "aTangent" },
				{ ShaderDataType::Float3, "aBitangent" }
			};
		}
	};

	struct VertexPTNTBC
	{
		glm::vec3 m_Position;
		glm::vec2 m_TexCoord;
		glm::vec3 m_Normal;
		glm::vec3 m_Tangent;
		glm::vec3 m_Bitangent;
		glm::vec4 m_Color;

		float* ToFloatArray() {
			float arr[18] = { m_Position.x, m_Position.y, m_Position.z, m_TexCoord.x, m_TexCoord.y, m_Normal.x, m_Normal.y, m_Normal.z, m_Tangent.x, m_Tangent.y, m_Tangent.z, m_Bitangent.x, m_Bitangent.y, m_Bitangent.z, m_Color.r, m_Color.g, m_Color.b, m_Color.a };
			return arr;
		}

		static BufferLayout GetBufferLayout() {
			return {
				{ ShaderDataType::Float3, "aPos" },
				{ ShaderDataType::Float2, "aTexCoords" },
				{ ShaderDataType::Float3, "aNormal" },
				{ ShaderDataType::Float3, "aTangent" },
				{ ShaderDataType::Float3, "aBitangent" },
				{ ShaderDataType::Float4, "aColor" }
			};
		}
	};

	struct MaterialSlot
	{
		std::string name;
		Ref<Material> material;
	};

	class Mesh: public Asset
	{

	public:
		Mesh() {}
		~Mesh() = default;

		virtual AssetType GetAssetType() const override { return AssetType::Mesh; }
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

		const Ref<SubMesh>& AddSubmesh(
			unsigned int materialIndex,
			std::vector<VertexPTNTB>& vertices,
			std::vector<unsigned int>& indices);

		void SetModelSource(const std::string& source) { this->modelSource = source; }
		const std::string& GetModelSource() const { return modelSource; };

		const std::vector<Ref<SubMesh>>& GetSubmeshes() const { return m_Submeshes; }

		void AddMaterial(unsigned int index, const Ref<Material> material, std::string slotName = "")
		{
			if (slotName == "") slotName = "Material " + std::to_string(m_Materials.size());

			if (m_Materials.size() <= index)
			{
				m_Materials.push_back({ slotName, material });
			}
			else 
			{
				m_Materials[index] = { slotName, material };
			}

		}

		void AddMaterial(const Ref<Material> material, std::string slotName = "") {
			if (slotName == "") slotName = "Material " + std::to_string(m_Materials.size());
			m_Materials.push_back({slotName, material}); 
		}

		void SetMaterial(unsigned int index, const Ref<Material>& material) { m_Materials[index].material = material; }

		const std::vector<MaterialSlot>& GetMaterials() const { return m_Materials; }
		Ref<Material> GetMaterial(unsigned int index) const { return m_Materials[index].material; }

		const Bounds3& GetBounds() const;
		void SetBounds(const Bounds3& bounds);

	private:
		std::string modelSource = "";
		std::vector<Ref<SubMesh>> m_Submeshes;
		std::vector<MaterialSlot> m_Materials;
		Bounds3 m_Bounds;
	};


}
