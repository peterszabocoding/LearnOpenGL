#pragma once

#include "Moongoose/Asset/Asset.h"
#include "Moongoose/Renderer/Texture.h"
#include "Moongoose/Renderer/Shader.h"

namespace Moongoose {

	class Material: public Asset
	{
	public:
		Material(const std::string& name, ShaderType shaderType = ShaderType::STATIC)
		{ 
			m_ID = UUID();
			m_Name = name;
			m_ShaderType = shaderType;
		}

		Material(UUID id, const std::string& name, ShaderType shaderType)
		{
			m_ID = id;
			m_Name = name;
			m_ShaderType = shaderType;
		}

		virtual AssetType getAssetType() const override { return AssetType::Material; }
		static AssetType GetStaticAssetType() { return AssetType::Material; }

		ShaderType getShaderType() const { return m_ShaderType; }
		const std::string& GetName() const { return m_Name; }
		void SetName(const std::string& newName) { m_Name = newName; }

		void bind() const { 
			if (m_Albedo) m_Albedo->bind(0);
		}

		void setAlbedo(const Ref<Texture2D>& albedo) {
			m_Albedo = albedo;
		}

		const Ref<Texture2D>& getAlbedo() const { return m_Albedo; }

	private:
		Ref<Texture2D> m_Albedo;
		ShaderType m_ShaderType;
	};

}

