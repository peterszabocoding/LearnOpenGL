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

		void Bind();
		void Unbind();
		void Resize(uint32_t width, uint32_t height);

		uint32_t GetWorldPositionTexutre();
		uint32_t GetViewPositionTexutre();
		uint32_t GetNormalTexture();
		uint32_t GetRoughnessTexture();

		uint32_t GetAttachment(int attachmentNumber);

		Framebuffer* GetGBuffer() const;
		const FramebufferSpecs& GetSpecs() const;

	private:
		FramebufferSpecs m_BufferSpecs;
		Ref<Framebuffer> m_Framebuffer;
	};
}