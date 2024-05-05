#pragma once

#include "Moongoose/Asset/Asset.h"
#include "Moongoose/Renderer/Texture.h"
#include "Moongoose/Renderer/Shader.h"

namespace Moongoose {

	class Material: public Asset
	{
	public:
		Material() { m_ID = UUID(); }
		Material(Ref<Shader> shader)
		{
			m_ID = UUID();
			m_Shader = shader;
		}

		virtual AssetType getAssetType() const override { return AssetType::Material; }
		static AssetType getStaticAssetType() { return AssetType::Material; }

		void bind() const { 
			m_Shader->Bind();
			m_Albedo->bind(0); 
		}

		void unbind() {
			m_Shader->Unbind();
		}

		void setAlbedo(Ref<Texture2D> albedo) {
			m_Albedo = albedo;
		}

		const Ref<Texture2D> getAlbedo() const { return m_Albedo; }

		void setShader(Ref<Shader> shader) {
			m_Shader = shader;
		}

		const Ref<Shader>& getShader() const { return m_Shader; }

	private:
		Ref<Texture2D> m_Albedo;
		Ref<Shader> m_Shader;
	};

}

