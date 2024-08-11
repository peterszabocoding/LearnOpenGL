#include "mgpch.h"
#include "GBuffer.h"

#include "FramebufferManager.h"
#include "glad/glad.h"

namespace Moongoose
{

	GBuffer::GBuffer(GBufferSpecs specs)
	{
		m_BufferSpecs = FramebufferSpecs();
		m_BufferSpecs.width = specs.width;
		m_BufferSpecs.height = specs.height;

		m_BufferSpecs.attachments = {
			FramebufferTextureFormat::RGBA16F,	// World Position
			FramebufferTextureFormat::RGBA16F,	// View Position
			FramebufferTextureFormat::RGBA16F,	// Normal
			FramebufferTextureFormat::RGBA8,	// Roughness
			FramebufferTextureFormat::DEPTH24
		};

		m_Framebuffer = FramebufferManager::CreateFramebuffer("GBuffer", m_BufferSpecs);
	}

	void GBuffer::Bind()
	{
		const int attachmentSize = m_BufferSpecs.attachments.Attachments.size();
		std::vector<unsigned int> attachments;
		attachments.reserve(attachmentSize);

		for (size_t i = 0; i < attachmentSize; i++) {
			attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
		}

		m_Framebuffer->Bind();
		glDrawBuffers(attachmentSize, attachments.data());
	}

	void GBuffer::Unbind()
	{
		m_Framebuffer->Unbind();
	}

	void GBuffer::Resize(uint32_t width, uint32_t height)
	{
		m_BufferSpecs.width = width;
		m_BufferSpecs.height = height;
		m_Framebuffer->Resize(width, height);
	}

	uint32_t GBuffer::GetWorldPositionTexutre()
	{
		return m_Framebuffer->GetColorAttachments()[0];
	}

	uint32_t GBuffer::GetViewPositionTexutre()
	{
		return m_Framebuffer->GetColorAttachments()[1];
	}

	uint32_t GBuffer::GetNormalTexture()
	{
		return m_Framebuffer->GetColorAttachments()[2];
	}

	uint32_t GBuffer::GetRoughnessTexture()
	{
		return m_Framebuffer->GetColorAttachments()[3];
	}

	uint32_t GBuffer::GetAttachment(int attachmentNumber)
	{
		const int attachment = attachmentNumber > m_BufferSpecs.attachments.Attachments.size()
			                       ? 0 : attachmentNumber;
		return m_Framebuffer->GetColorAttachments()[attachment];
	}

	Framebuffer* GBuffer::GetGBuffer() const
	{
		return m_Framebuffer.get();
	}

	const FramebufferSpecs& GBuffer::GetSpecs() const
	{
		return m_BufferSpecs;
	}
}

