#pragma once

#include "Moongoose/Asset/Asset.h"
#include "Moongoose/Asset/UUID.h"
#include "Moongoose/Renderer/Buffer.h"
#include <glm/glm.hpp>

namespace Moongoose {

	enum class TextureFormat
	{
		None = 0,
		RED8UN,
		RED8UI,
		RED16UI,
		RED32UI,
		RED32F,
		RG8,
		RG16F,
		RG32F,
		RGB,
		RGBA,
		RGBA16F,
		RGBA32F,

		B10R11G11UF,

		SRGB,

		DEPTH32FSTENCIL8UINT,
		DEPTH32F,
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8,
	};

	enum class TextureWrap
	{
		None = 0,
		Clamp,
		Repeat
	};

	enum class TextureFilter
	{
		None = 0,
		Linear,
		Nearest,
		Cubic
	};

	enum class TextureType
	{
		None = 0,
		Texture2D,
		TextureCube
	};

	struct TextureSpecs {
		uint32_t		Width = 0;
		uint32_t		Height = 0;
		uint32_t		BitDepth = 0;
		std::string		FileLocation = "";
		TextureWrap		TextureWrap = TextureWrap::Repeat;
		TextureFilter	TextureFilter = TextureFilter::Linear;
		TextureFormat	TextureFormat = TextureFormat::RGB;
	};

	namespace Utils {

		inline TextureType TextureTypeFromString(const std::string& textureType)
		{
			if (textureType == "None")				return TextureType::None;
			if (textureType == "Texture2D")			return TextureType::Texture2D;
			if (textureType == "TextureCube")		return TextureType::TextureCube;

			MG_ASSERT(false, "Unknown Texture Type");
			return TextureType::None;
		}
		inline const char* TextureTypeToString(TextureType textureType)
		{
			switch (textureType)
			{
			case TextureType::None:				return "None";
			case TextureType::Texture2D:		return "Texture2D";
			case TextureType::TextureCube:		return "TextureCube";
			}

			MG_ASSERT(false, "Unknown Texture Type");
			return "None";
		}

		inline TextureFilter TextureFilterFromString(const std::string& textureType)
		{
			if (textureType == "None")			return TextureFilter::None;
			if (textureType == "Linear")		return TextureFilter::Linear;
			if (textureType == "Nearest")		return TextureFilter::Nearest;
			if (textureType == "Cubic")			return TextureFilter::Cubic;

			MG_ASSERT(false, "Unknown Texture Filter");
			return TextureFilter::None;
		}
		inline const char* TextureFilterToString(TextureFilter textureType)
		{
			switch (textureType)
			{
			case TextureFilter::None:			return "None";
			case TextureFilter::Linear:			return "Linear";
			case TextureFilter::Nearest:		return "Nearest";
			case TextureFilter::Cubic:			return "Cubic";
			}

			MG_ASSERT(false, "Unknown Texture Filter");
			return "None";
		}

		inline TextureWrap TextureWrapFromString(const std::string& textureWrap)
		{
			if (textureWrap == "None")			return TextureWrap::None;
			if (textureWrap == "Clamp")			return TextureWrap::Clamp;
			if (textureWrap == "Repeat")		return TextureWrap::Repeat;

			MG_ASSERT(false, "Unknown Texture Wrap");
			return TextureWrap::None;
		}
		inline const char* TextureWrapToString(TextureWrap textureWrap)
		{
			switch (textureWrap)
			{
			case TextureWrap::None:			return "None";
			case TextureWrap::Clamp:		return "Clamp";
			case TextureWrap::Repeat:		return "Repeat";
			}

			MG_ASSERT(false, "Unknown Texture Wrap");
			return "None";
		}
	}

	class Texture: public Asset {
	public:
		virtual void bind(uint32_t slot = 0) const = 0;

		virtual TextureFormat getFormat() const = 0;
		virtual uint32_t getWidth() const = 0;
		virtual uint32_t getHeight() const = 0;
		virtual glm::uvec2 getSize() const = 0;
		virtual TextureType getType() const = 0;
	};

	class Texture2D : public Texture {
	public:
		static Ref<Texture2D> Create();

		virtual AssetType GetAssetType() const override { return AssetType::Texture; }
		static AssetType GetStaticAssetType() { return AssetType::Texture; }

		virtual void Resize(const glm::uvec2& size) = 0;
		virtual void Resize(const uint32_t width, const uint32_t height) = 0;

		virtual void LoadData(TextureSpecs specs, void* data) = 0;
		virtual void UnloadData() = 0;

		virtual bool IsLoaded() const = 0;
		virtual const std::string& GetPath() const = 0;

		virtual void* GetPointerToData() = 0;
		virtual Buffer GetBuffer() = 0;

		virtual TextureType getType() const override { return TextureType::Texture2D; }

		TextureWrap GetTextureWrap() const { return m_TextureSpecs.TextureWrap; }
		void SetTextureWrap(const TextureWrap textureWrap)
		{
			m_TextureSpecs.TextureWrap = textureWrap;
		} 

		TextureFilter GetTextureFilter() const { return m_TextureSpecs.TextureFilter; }
		void SetTextureFilter(const TextureFilter textureFilter)
		{
			m_TextureSpecs.TextureFilter = textureFilter;
		}

		TextureFormat GetTextureFormat() const { return m_TextureFormat; }

	protected:
		TextureSpecs m_TextureSpecs;
		TextureFormat m_TextureFormat = TextureFormat::None;
	};

	class TextureCube : public Texture {

	};

}