#pragma once

#include "Moongoose/Asset/Asset.h"
#include "Moongoose/Renderer/Texture.h"
#include "Moongoose/Renderer/Shader.h"

namespace Moongoose {

	class Material: public Asset
	{
	public:
		explicit Material(const std::string& name, const ShaderType shaderType = ShaderType::STATIC)
		{ 
			m_Id = UUID();
			m_Name = name;
			m_ShaderType = shaderType;
		}

		Material(const UUID& id, const std::string& name, const ShaderType shaderType)
		{
			m_Id = id;
			m_Name = name;
			m_ShaderType = shaderType;
		}

		virtual AssetType GetAssetType() const override { return AssetType::Material; }
		static AssetType GetStaticAssetType() { return AssetType::Material; }

		ShaderType GetShaderType() const { return m_ShaderType; }
		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& newName) { m_Name = newName; }

		void Bind() const { 
			if (m_Albedo) m_Albedo->bind(0);
		}

		void SetAlbedo(const Ref<Texture2D>& albedo) {
			m_Albedo = albedo;
		}

		const Ref<Texture2D>& GetAlbedo() const { return m_Albedo; }

	private:
		Ref<Texture2D> m_Albedo;
		ShaderType m_ShaderType;
	};

}

