#pragma once

#include <glm/glm.hpp>
#include "Moongoose/Log.h"

namespace Moongoose {

	enum class FramebufferTextureFormat: uint32_t {
		None = 0,

		// Color
		RGBA8,
		RGBA16,
		RGBA16F,
		RGBA32F,
		RED_INTEGER,

		// Depth
		DEPTH24,
		// Depth / Stencil
		DEPTH24STENCIL8,

		// Defaults
		Depth = DEPTH24STENCIL8
	};

	namespace Util {

		static std::string FramebufferTextureFormatToString(FramebufferTextureFormat format) {
			switch (format) {
				case FramebufferTextureFormat::RGBA8: return "RGBA8";
				case FramebufferTextureFormat::RGBA16: return "RGBA16";
				case FramebufferTextureFormat::RGBA16F: return "RGBA16F";
				case FramebufferTextureFormat::RGBA32F: return "RGBA32F";
				case FramebufferTextureFormat::RED_INTEGER: return "RED_INTEGER";
				case FramebufferTextureFormat::DEPTH24: return "DEPTH24";
				case FramebufferTextureFormat::DEPTH24STENCIL8: return "DEPTH24STENCIL8";
			}
			return "Unknown";
		}
	}

	struct FramebufferTextureSpecs {
		FramebufferTextureSpecs() = default;
		FramebufferTextureSpecs(FramebufferTextureFormat format) : TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat;
	};

	struct FramebufferAttachmentSpecs {
		FramebufferAttachmentSpecs() = default;
		FramebufferAttachmentSpecs(const std::initializer_list<FramebufferTextureSpecs> attachments) : Attachments(attachments) {}

		std::vector<FramebufferTextureSpecs> Attachments;
	};

	struct FramebufferSpecs {
		uint32_t Width, Height;
		uint32_t Samples = 1;

		bool HasShadowMapAttachment = false;
		bool SwapChainTarget = false;

		glm::vec4 ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		FramebufferAttachmentSpecs Attachments;
	};

	class Framebuffer {

	public:
		Framebuffer(const std::string& name, const FramebufferSpecs& specs);
		~Framebuffer();

		void Bind();
		void Bind(uint32_t viewportWidth, uint32_t viewportHeight);
		void Unbind();
		void Resize(uint32_t width, uint32_t height);
		int ReadPixel(uint32_t attachmentIndex, int x, int y);
		void ClearAttachment(uint32_t attachmentIndex, int value);

		FramebufferSpecs GetSpecs() const { return m_Specs; }

		const std::vector<uint32_t> GetColorAttachments() const { return m_ColorAttachments; }
		std::vector<uint32_t>& GetColorAttachments() { return m_ColorAttachments; }

		uint32_t GetDepthAttachment() const { return m_DepthAttachment; }
		uint32_t GetShadowMapAttachmentID() const { return m_ShadowMapAttachment; }
		glm::vec2 GetResolution() const { return { m_Specs.Width, m_Specs.Height }; }

	private:
		void Invalidate();
		void DeleteBuffer();
		void AddShadowMapAttachment();

		static int TextureFormatToGL(const FramebufferTextureFormat format);
		static int TextureFormatToInternalGL(const FramebufferTextureFormat format);

	public:
		std::string m_Name;
		FramebufferSpecs m_Specs;
		unsigned int m_FramebufferID = 0;

		std::vector<uint32_t> m_ColorAttachments;
		uint32_t m_DepthAttachment = 0;
		uint32_t m_ShadowMapAttachment = 0;

		std::vector<FramebufferTextureSpecs> m_ColorAttachmentSpecs;
		FramebufferTextureSpecs m_DepthAttachmentSpec = FramebufferTextureFormat::None;
	};

}