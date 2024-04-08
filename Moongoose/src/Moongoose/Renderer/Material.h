#pragma once

#include <string>
#include "Shader.h"
#include "Resource.h"

class Texture;

namespace Moongoose {

	struct MaterialSpecs {
		std::string materialId;
		ShaderType shaderType = ShaderType::STATIC;
	};

	class Material: public Resource {

	public:
		Material(MaterialSpecs specs): Resource(specs.materialId, ResourceType::MATERIAL)
		{
			m_Albedo = nullptr;
			m_Normal = nullptr;
			m_Roughness = nullptr;
			m_Metallic = nullptr;
			m_Specs = specs;
		}

		~Material() = default;

		const std::string const getMaterialId() const { return m_Specs.materialId; }
		const ShaderType const GetShaderType() const { return m_Specs.shaderType; }
		const MaterialSpecs& const GetSpecs() const { return m_Specs; }

		void SetShaderType(ShaderType type) { m_Specs.shaderType = type; }

		void SetAlbedo(Texture* texture) { m_Albedo = texture; }
		void SetNormal(Texture* texture) { m_Normal = texture; }
		void SetRoughness(Texture* texture) { m_Roughness = texture; }
		void SetMetallic(Texture* texture) { m_Metallic = texture; }

		const Texture* GetAlbedo() const { return m_Albedo; }
		const Texture* GetNormal() const { return m_Normal; }
		const Texture* GetRoughness() const { return m_Roughness; }
		const Texture* GetMetallic() const { return m_Metallic; }

	private:
		MaterialSpecs m_Specs;

		Texture* m_Albedo;
		Texture* m_Normal;
		Texture* m_Roughness;
		Texture* m_Metallic;
	};


}