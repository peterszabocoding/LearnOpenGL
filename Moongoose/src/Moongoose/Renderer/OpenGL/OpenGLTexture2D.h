#pragma once
#include <glad/glad.h>
#include "Moongoose/Renderer/Texture.h"

namespace Moongoose {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D();
		virtual ~OpenGLTexture2D();

		virtual void bind(uint32_t textureUnit = 0) const override;

		virtual void Resize(const glm::uvec2& size) override;
		virtual void Resize(const uint32_t width, const uint32_t height) override;

		virtual void LoadData(TextureSpecs specs, void* data) override;
		virtual void UnloadData() override;

		virtual bool IsLoaded() const override;

		virtual const std::string& GetPath() const override { return m_TextureSpecs.FileLocation; };
		virtual TextureFormat getFormat() const override { return m_TextureFormat; }
		virtual uint32_t getWidth() const override { return m_TextureSpecs.Width; }
		virtual uint32_t getHeight() const override { return m_TextureSpecs.Height; }
		virtual glm::uvec2 getSize() const override { return { m_TextureSpecs.Width, m_TextureSpecs.Height }; }
		virtual Buffer GetBuffer() override { return m_TextureData; }
		virtual void* GetPointerToData() { return (void*) m_TextureID; }

	private:
		GLuint m_TextureID;
		Buffer m_TextureData;
	};

}

