#pragma once
#include <glad/glad.h>
#include "Moongoose/Renderer/Texture.h"

namespace Moongoose {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(TextureSpecs specs);
		virtual ~OpenGLTexture2D();

		virtual void bind(uint32_t textureUnit = 0) const override;

		virtual void resize(const glm::uvec2& size) override;
		virtual void resize(const uint32_t width, const uint32_t height) override;

		virtual void loadData(void* data, uint32_t width, uint32_t height, uint8_t bitDepth) override;

		virtual bool isLoaded() const override;

		virtual const std::string& getPath() const override { return m_TextureSpecs.FileLocation; };
		virtual TextureFormat getFormat() const override { return m_TextureFormat; }
		virtual uint32_t getWidth() const override { return m_TextureSpecs.Width; }
		virtual uint32_t getHeight() const override { return m_TextureSpecs.Height; }
		virtual glm::uvec2 getSize() const override { return { m_TextureSpecs.Width, m_TextureSpecs.Height }; }
		virtual Buffer getBuffer() override { return m_TextureData; }

	private:
		GLuint m_TextureID;
		TextureSpecs m_TextureSpecs;
		Buffer m_TextureData;
		TextureFormat m_TextureFormat = TextureFormat::None;
	};

}

