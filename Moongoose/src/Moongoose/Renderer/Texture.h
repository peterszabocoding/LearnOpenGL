#pragma once

#include <glad/glad.h>
#include "Moongoose/Util/UUID.h"

namespace Moongoose {

	struct TextureSpecs {
		uint32_t		Width;
		uint32_t		Height;
		uint32_t		BitDepth;
		std::string		FileLocation;
		uint32_t		InternalFormat;
		uint32_t		DataFormat;
		bool			MipmapEnabled = true;
		uint32_t		MaxMipmapLevel = 4;
	};

	class Texture {
	public:

		Texture(UUID uuid, TextureSpecs specs) {
			textureID = 0;
			m_Specs = specs;
		}

		Texture(const char* fileLoc) {
			textureID = 0;
			m_Specs = TextureSpecs{};
			m_Specs.Width = 0;
			m_Specs.Height = 0;
			m_Specs.BitDepth = 0;
			m_Specs.FileLocation = fileLoc;
			m_Specs.InternalFormat = GL_RGBA8;
			m_Specs.DataFormat = GL_RGBA;
		}

		GLuint GetTextureID() const { return textureID; }
		const TextureSpecs& GetSpecs() const { return m_Specs; }
		TextureSpecs GetSpecs() { return m_Specs; }
		GLenum GetInternalFormat() const { return m_Specs.InternalFormat; }
		GLenum GetDataFormat() const { return m_Specs.DataFormat; }

		const std::string& GetSourceFilePath() const { return m_Specs.FileLocation; }

		virtual ~Texture() {
			ClearTexture();
		}

		virtual bool operator==(const Texture& other) const {
			return textureID == other.GetTextureID();
		}

	private:
		void ClearTexture() {
			glDeleteTextures(1, &textureID);
			textureID = 0;
		}

	protected:
		GLuint textureID;
		TextureSpecs m_Specs;
	};

}