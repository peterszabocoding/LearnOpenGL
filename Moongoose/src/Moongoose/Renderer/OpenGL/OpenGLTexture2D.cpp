#include "mgpch.h"
#include "OpenGLTexture2D.h"

#include "Moongoose/Log.h"

namespace Moongoose {

	OpenGLTexture2D::OpenGLTexture2D(): m_TextureID(0) {}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_TextureID);
		m_TextureID = 0;
	}

	void OpenGLTexture2D::Bind(const uint32_t textureUnit) const
	{
		glActiveTexture(GL_TEXTURE0 + textureUnit);
		glBindTexture(GL_TEXTURE_2D, m_TextureID);
	}

	void OpenGLTexture2D::Resize(const glm::uvec2& size){}

	void OpenGLTexture2D::Resize(const uint32_t width, const uint32_t height) {}

	void OpenGLTexture2D::LoadData(const TextureSpecs specs, void* data)
	{
		m_TextureSpecs = specs;
		m_TextureFormat = specs.textureFormat;

		m_TextureData.Data = data;
		m_TextureData.Size = m_TextureSpecs.width * m_TextureSpecs.height * m_TextureSpecs.bitDepth;

		GLenum internalFormat = 0, dataFormat = 0;

		if (m_TextureSpecs.bitDepth == 1) {
			internalFormat = GL_R8;
			dataFormat = GL_RED;
			LOG_CORE_INFO("OpenGLTexture2D.cpp | Loaded texture: {0} (Red integer)", m_TextureSpecs.fileLocation);
		}
		else if (m_TextureSpecs.bitDepth == 4) {
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
			LOG_CORE_INFO("OpenGLTexture2D.cpp | Loaded texture: {0} (RGBA)", m_TextureSpecs.fileLocation);
		}
		else if (m_TextureSpecs.bitDepth == 3) {
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
			LOG_CORE_INFO("OpenGLTexture2D.cpp | Loaded texture: {0} (RGB)", m_TextureSpecs.fileLocation);
		}

		glCreateTextures(GL_TEXTURE_2D, 1, &m_TextureID);
		glTextureStorage2D(m_TextureID, 1, internalFormat, m_TextureSpecs.width, m_TextureSpecs.height);
		glTextureSubImage2D(m_TextureID, 0, 0, 0, m_TextureSpecs.width, m_TextureSpecs.height, dataFormat, GL_UNSIGNED_BYTE, m_TextureData.Data);

		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_TextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTextureParameteri(m_TextureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_TextureID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	void OpenGLTexture2D::UnloadData()
	{
		glDeleteTextures(1, &m_TextureID);

		m_TextureID = 0;
		m_TextureData.Data = nullptr;
		m_TextureData.Size = 0;
	}

	bool OpenGLTexture2D::IsLoaded() const
	{
		return m_TextureData;
	}

}
