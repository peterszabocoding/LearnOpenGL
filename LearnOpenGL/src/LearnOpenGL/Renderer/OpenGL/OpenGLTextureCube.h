#pragma once
#include <glad/glad.h>
#include "LearnOpenGL/Renderer/Texture.h"

namespace LearnOpenGL
{

	class OpenGLTextureCube : public TextureCube
	{
	public:
		OpenGLTextureCube();
		~OpenGLTextureCube() override;

		virtual void Bind(uint32_t textureUnit = 0) const override;

		virtual void LoadData(TextureSpecs specs, void* data) override;
		virtual void UnloadData() override;

		virtual void* GetPointerToData() override;
		virtual Buffer GetBuffer() override;

	private:
		unsigned int m_TextureId;
		Buffer m_TextureData;
	};

}

