#include "pch.h"
#include "OpenGLTextureCube.h"

namespace LearnOpenGL
{

	OpenGLTextureCube::OpenGLTextureCube() : m_TextureId(0) {}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		glDeleteTextures(1, &m_TextureId);
		m_TextureId = 0;
	}

	void OpenGLTextureCube::Bind(uint32_t textureUnit) const
	{
	}

	void OpenGLTextureCube::LoadData(TextureSpecs specs, void* data)
	{
		m_TextureSpecs = specs;

		glGenTextures(1, &m_TextureId);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
				0, 
				GL_DEPTH_COMPONENT, 
				m_TextureSpecs.width, 
				m_TextureSpecs.height, 
				0, 
				GL_DEPTH_COMPONENT,
				GL_FLOAT, 
				nullptr);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			
	}

	void OpenGLTextureCube::UnloadData()
	{
	}

	void* OpenGLTextureCube::GetPointerToData()
	{
		return nullptr;
	}

	Buffer OpenGLTextureCube::GetBuffer()
	{
		return m_TextureData;
	}
}
 