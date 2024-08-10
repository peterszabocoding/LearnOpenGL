#pragma once

#include "Moongoose/Asset/Asset.h"
#include "Moongoose/Renderer/Texture.h"
#include "Moongoose/Renderer/Shader.h"

namespace Moongoose {

	class Material: public Asset
	{
	public:
		explicit Material(const std::string& name, const ShaderType shaderType = ShaderType::STATIC): Material(UUID(), name, shaderType) {}

		Material(const UUID& id, const std::string& name, const ShaderType shaderType)
		{
			m_Id = id;
			m_Name = name;
			m_ShaderType = shaderType;

			m_AlbedoColor = glm::vec3(1.0f);
			m_MetallicValue = 0.0f;
			m_RoughnessValue = 1.0f;
		}

		virtual AssetType GetAssetType() const override { return AssetType::Material; }
		static AssetType GetStaticAssetType() { return AssetType::Material; }

		[[nodiscard]] ShaderType GetShaderType() const { return m_ShaderType; }
		[[nodiscard]] const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& newName) { m_Name = newName; }

		void Bind() const { 
			if (m_Albedo) m_Albedo->Bind(0);
			if (m_Normal) m_Normal->Bind(1);
			if (m_Metallic) m_Metallic->Bind(2);
			if (m_Roughness) m_Roughness->Bind(3);
		}

	public:
		Ref<Texture2D> m_Albedo;
		Ref<Texture2D> m_Normal;
		Ref<Texture2D> m_Metallic;
		Ref<Texture2D> m_Roughness;

		glm::vec3 m_AlbedoColor;
		float m_MetallicValue;
		float m_RoughnessValue;

	private:
		ShaderType m_ShaderType;
	};
}
