#include "mgpch.h"
#include "Texture.h"

namespace Moongoose{

	Texture::Texture(TextureSpecs specs)
	{
		m_TextureID = 0;
		m_Specs = specs;
	}

	Texture::Texture(const char* fileLoc)
	{
		m_TextureID = 0;
		m_Specs = TextureSpecs{};
		m_Specs.Width = 0;
		m_Specs.Height = 0;
		m_Specs.BitDepth = 0;
		m_Specs.FileLocation = fileLoc;
		m_Specs.InternalFormat = GL_RGBA8;
		m_Specs.DataFormat = GL_RGBA;
	}

	Texture::~Texture()
	{
		clearTexture();
	}

	void Texture::clearTexture()
	{
		glDeleteTextures(1, &m_TextureID);
		m_TextureID = 0;
	}

}