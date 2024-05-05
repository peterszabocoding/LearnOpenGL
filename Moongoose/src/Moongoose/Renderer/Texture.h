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
		static Ref<Texture2D> Create(TextureSpecs specs);

		virtual void resize(const glm::uvec2& size) = 0;
		virtual void resize(const uint32_t width, const uint32_t height) = 0;

		virtual void loadData(void* data, uint32_t width, uint32_t height, uint8_t bitDepth) = 0;

		//virtual Ref<Image2D> GetImage() const = 0;

		virtual Buffer getBuffer() = 0;

		virtual bool isLoaded() const = 0;
		virtual const std::string& getPath() const = 0;

		virtual void* getPointerToData() = 0;

		virtual TextureType getType() const override { return TextureType::Texture2D; }
		virtual AssetType getAssetType() const override { return AssetType::Texture; }
		static AssetType GetStaticAssetType() { return AssetType::Texture; }
	};

	class TextureCube : public Texture {

	};

}