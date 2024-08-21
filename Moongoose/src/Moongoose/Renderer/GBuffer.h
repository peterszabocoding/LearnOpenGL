#pragma once
#include "Framebuffer.h"
#include "Moongoose/Core.h"

namespace Moongoose
{
	class GBuffer
	{
	public:
		struct GBufferSpecs
		{
			uint32_t width, height;
		};

	public:
		GBuffer() = default;
		GBuffer(GBufferSpecs specs);

		void Bind() const;
		void Unbind() const;
		void Resize(uint32_t width, uint32_t height);

		uint32_t GetWorldPositionTexture() const;
		uint32_t GetViewPositionTexture() const;
		uint32_t GetNormalTexture() const;
		uint32_t GetRoughnessTexture() const;

		uint32_t GetAttachment(int attachmentNumber) const;
		uint32_t GetDepthAttachment() const;

		Framebuffer* GetFramebuffer() const;
		const FramebufferSpecs& GetSpecs() const;

	private:
		FramebufferSpecs m_BufferSpecs;
		Ref<Framebuffer> m_Framebuffer;
	};
}
