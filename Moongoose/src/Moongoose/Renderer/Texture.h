#pragma once

#include <glad/glad.h>
#include "Moongoose/Asset/Asset.h"
#include "Moongoose/Asset/UUID.h"

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

	class Texture: Asset {
	public:

		Texture(TextureSpecs specs);
		Texture(const char* fileLoc);
		virtual ~Texture();

		GLuint getTextureID() const { return m_TextureID; }
		const TextureSpecs& getSpecs() const { return m_Specs; }
		TextureSpecs getSpecs() { return m_Specs; }

		GLenum getInternalFormat() const { return m_Specs.InternalFormat; }
		GLenum getDataFormat() const { return m_Specs.DataFormat; }

		const std::string& getSourceFilePath() const { return m_Specs.FileLocation; }

		virtual bool operator==(const Texture& other) const {
			return m_TextureID == other.getTextureID();
		}

	private:
		void clearTexture();

	protected:
		GLuint m_TextureID;
		TextureSpecs m_Specs;
	};

}