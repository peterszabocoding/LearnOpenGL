#include "pch.h"
#include "GBuffer.h"

#include "FramebufferManager.h"
#include "glad/glad.h"

namespace LearnOpenGL
{
	GBuffer::GBuffer(const GBufferSpecs specs)
	{
		m_BufferSpecs = FramebufferSpecs();
		m_BufferSpecs.width = specs.width;
		m_BufferSpecs.height = specs.height;

		m_BufferSpecs.attachments = {
			FramebufferTextureFormat::RGBA16F, // World Position
			FramebufferTextureFormat::RGBA16F, // View Position
			FramebufferTextureFormat::RGBA16F, // Normal
			FramebufferTextureFormat::RGBA8, // Roughness
			FramebufferTextureFormat::RED_INTEGER,
			FramebufferTextureFormat::DEPTH24,

		};

		m_Framebuffer = FramebufferManager::CreateFramebuffer("GBuffer");
		m_Framebuffer->Configure(m_BufferSpecs);
	}

	void GBuffer::Bind() const
	{
		const int attachmentSize = m_BufferSpecs.attachments.Attachments.size();
		std::vector<unsigned int> attachments;
		attachments.reserve(attachmentSize);

		for (size_t i = 0; i < attachmentSize; i++) attachments.push_back(GL_COLOR_ATTACHMENT0 + i);

		m_Framebuffer->Bind();
		glDrawBuffers(attachmentSize, attachments.data());
	}

	void GBuffer::Unbind() const
	{
		m_Framebuffer->Unbind();
	}

	void GBuffer::Resize(const uint32_t width, const uint32_t height)
	{
		if (m_BufferSpecs.width != width || m_BufferSpecs.height != height)
		{
			m_BufferSpecs.width = width;
			m_BufferSpecs.height = height;
			m_Framebuffer->Resize(width, height);
		}
	}

	uint32_t GBuffer::GetWorldPositionTexture() const
	{
		return m_Framebuffer->GetColorAttachments()[0];
	}

	uint32_t GBuffer::GetViewPositionTexture() const
	{
		return m_Framebuffer->GetColorAttachments()[1];
	}

	uint32_t GBuffer::GetNormalTexture() const
	{
		return m_Framebuffer->GetColorAttachments()[2];
	}

	uint32_t GBuffer::GetRoughnessTexture() const
	{
		return m_Framebuffer->GetColorAttachments()[3];
	}

	uint32_t GBuffer::GetAttachment(const int attachmentNumber) const
	{
		const int attachment = attachmentNumber > m_BufferSpecs.attachments.Attachments.size() ? 0 : attachmentNumber;
		return m_Framebuffer->GetColorAttachments()[attachment];
	}

	uint32_t GBuffer::GetDepthAttachment() const
	{
		return m_Framebuffer->GetDepthAttachment();
	}

	int GBuffer::ReadPixel(const uint32_t attachmentIndex, const int x, const int y) const
	{
		MG_CORE_ASSERT(attachmentIndex < m_Framebuffer->GetColorAttachments().size(),
		               "attachmentIndex exeeded number of color attachments")
		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData = -1;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

		return pixelData;
	}

	Framebuffer* GBuffer::GetFramebuffer() const
	{
		return m_Framebuffer.get();
	}

	const FramebufferSpecs& GBuffer::GetSpecs() const
	{
		return m_BufferSpecs;
	}
}
