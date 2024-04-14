#include "mgpch.h"
#include "OpenGLTexture2D.h"
#include "Moongoose/Log.h"

namespace Moongoose {

	OpenGLTexture2D::OpenGLTexture2D(TextureSpecs specs)
	{
		m_TextureSpecs = specs;
		m_TextureFormat = specs.TextureFormat;
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_TextureID);
		m_TextureID = 0;
	}

	void OpenGLTexture2D::bind(uint32_t textureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}

	void OpenGLTexture2D::resize(const glm::uvec2& size)
	{
	}

	void OpenGLTexture2D::resize(const uint32_t width, const uint32_t height)
	{
	}

	void OpenGLTexture2D::loadData(void* data, uint32_t width, uint32_t height, uint8_t bitDepth)
	{
		m_TextureData.Data = data;
		m_TextureData.Size = width * height * bitDepth;

		GLenum internalFormat = 0, dataFormat = 0;

		if (m_TextureSpecs.BitDepth == 1) {
			internalFormat = GL_R8;
			dataFormat = GL_RED;
			LOG_CORE_INFO("OpenGLTexture2D.cpp | Loaded texture: {0} (Red integer)", m_TextureSpecs.FileLocation);
		}
		else if (m_TextureSpecs.BitDepth == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
			LOG_CORE_INFO("OpenGLTexture2D.cpp | Loaded texture: {0} (RGBA)", m_TextureSpecs.FileLocation);
		}
		else if (m_TextureSpecs.BitDepth == 3) {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
			LOG_CORE_INFO("OpenGLTexture2D.cpp | Loaded texture: {0} (RGB)", m_TextureSpecs.FileLocation);
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
		glTextureStorage2D(m_TextureID, 1, internalFormat, m_TextureSpecs.Width, m_TextureSpecs.Height);
		glTextureSubImage2D(m_TextureID, 0, 0, 0, m_TextureSpecs.Width, m_TextureSpecs.Height, dataFormat, GL_UNSIGNED_BYTE, m_TextureData.Data);

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	bool OpenGLTexture2D::isLoaded() const
	{
		return m_TextureData;
	}

}
