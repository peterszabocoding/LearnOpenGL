#include "mgpch.h"
#include <glad/glad.h>
#include "Framebuffer.h"
#include "RenderCommand.h"

namespace Moongoose {

	namespace Utils {

		static bool IsDepthFormat(FramebufferTextureFormat format) {
			switch (format) {
			case FramebufferTextureFormat::DEPTH24: return true;
			case FramebufferTextureFormat::DEPTH24STENCIL8: return true;
			}
			return false;
		}

		static GLenum TextureTarget(bool multisampled) {
			return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
		}

		static void CreateTextures(bool multisampled, uint32_t* outID, uint32_t count) {
			glCreateTextures(TextureTarget(multisampled), count, outID);
		}

		static void BindTexture(bool multisampled, uint32_t id) {
			glBindTexture(TextureTarget(multisampled), id);
		}

		static void AttachColorTexture(uint32_t id, int samples, GLenum internalFormat, GLenum format, uint32_t width, uint32_t height, int index) {
			bool multisampled = samples > 1;
			if (multisampled) {
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else {
				glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, TextureTarget(multisampled), id, 0);
		}

		static void AttachDepthTexture(uint32_t id, int samples, GLenum format, GLenum attachmentType, uint32_t width, uint32_t height) {
			bool multisampled = samples > 1;
			if (multisampled) {
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else {
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, TextureTarget(multisampled), id, 0);
		}

		static void AttachShadowMapTexture(uint32_t id, int samples, GLenum format, uint32_t width, uint32_t height) {
			bool multisampled = samples > 1;
			if (multisampled) {
				glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, format, width, height, GL_FALSE);
			}
			else {
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			}

			GLfloat bColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bColor);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, TextureTarget(multisampled), id, 0);
		}

		static void LogFramebufferError(GLenum framebufferError) {
			std::string errorType;
			switch (framebufferError) {
			case GL_FRAMEBUFFER_UNDEFINED:
				errorType = "GL_FRAMEBUFFER_UNDEFINED";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
				errorType = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
				errorType = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
				errorType = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";
				break;
			case GL_FRAMEBUFFER_UNSUPPORTED:
				errorType = "GL_FRAMEBUFFER_UNSUPPORTED";
				break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
				errorType = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";
				break;
			default:
				errorType = "UNKNOWN";
				break;
			}

			LOG_CORE_ERROR("Framebuffer error: {0}", errorType);
		}
	}

	Framebuffer::Framebuffer(const std::string& name, const FramebufferSpecs& specs) : m_Name(name), m_Specs(specs)
	{
		for (auto& format : m_Specs.Attachments.Attachments) {
			if (!Utils::IsDepthFormat(format.TextureFormat)) {
				m_ColorAttachmentSpecs.emplace_back(format.TextureFormat);
			} else {
				m_DepthAttachmentSpec = format.TextureFormat;
			}
		}

		Invalidate();
	}

	Framebuffer::~Framebuffer()
	{
		DeleteBuffer();
	}

	void Framebuffer::Bind() {

		int attachmentSize = m_ColorAttachments.size();
		std::vector<unsigned int> attachments;
		attachments.reserve(attachmentSize);

		for (size_t i = 0; i < attachmentSize; i++) {
			attachments.push_back(GL_COLOR_ATTACHMENT0 + i);
		}

		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		glDrawBuffers(attachmentSize, attachments.data());
		RenderCommand::SetViewport(0, 0, m_Specs.Width, m_Specs.Height);
	}

	void Framebuffer::Bind(uint32_t viewportWidth, uint32_t viewportHeight)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		RenderCommand::SetViewport(0, 0, viewportWidth, viewportHeight);
	}

	void Framebuffer::Unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Specs.Width = width;
		m_Specs.Height = height;
		Invalidate();
	}

	int Framebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y)
	{
		MG_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "attachmentIndex exeeded number of color attachments");

		glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
		int pixelData = -1;
		glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);

		return pixelData;
	}

	void Framebuffer::ClearAttachment(uint32_t attachmentIndex, int value)
	{
		MG_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size(), "attachmentIndex exeeded number of color attachments");

		auto& spec = m_ColorAttachmentSpecs[attachmentIndex];
		glClearTexImage(m_ColorAttachments[attachmentIndex],
			0,
			GL_RED_INTEGER, 
			GL_INT, 
			&value);
	}

	void Framebuffer::Invalidate()
	{
		DeleteBuffer();
		glCreateFramebuffers(1, &m_FramebufferID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);

		bool multisample = m_Specs.Samples > 1;

		if (m_ColorAttachmentSpecs.size()) {

			m_ColorAttachments.resize(m_ColorAttachmentSpecs.size());
			Utils::CreateTextures(multisample, m_ColorAttachments.data(), m_ColorAttachments.size());

			for (size_t i = 0; i < m_ColorAttachments.size(); i++) {
				Utils::BindTexture(multisample, m_ColorAttachments[i]);
				
				Utils::AttachColorTexture(
					m_ColorAttachments[i], 
					m_Specs.Samples, 
					TextureFormatToInternalGL(m_ColorAttachmentSpecs[i].TextureFormat),
					TextureFormatToGL(m_ColorAttachmentSpecs[i].TextureFormat),
					m_Specs.Width, 
					m_Specs.Height, 
					i);
			}
		}

		if (m_DepthAttachmentSpec.TextureFormat != FramebufferTextureFormat::None) {
			Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
			Utils::BindTexture(multisample, m_DepthAttachment);
			switch (m_DepthAttachmentSpec.TextureFormat) {
			case FramebufferTextureFormat::DEPTH24STENCIL8:
				Utils::AttachDepthTexture(m_DepthAttachment, m_Specs.Samples, GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT, m_Specs.Width, m_Specs.Height);
				break;
			case FramebufferTextureFormat::DEPTH24:
				Utils::AttachDepthTexture(m_DepthAttachment, m_Specs.Samples, GL_DEPTH_COMPONENT24, GL_DEPTH_ATTACHMENT, m_Specs.Width, m_Specs.Height);
				break;
			}
		}

		if (m_Specs.HasShadowMapAttachment) AddShadowMapAttachment();

		GLenum framebufferError = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (framebufferError != GL_FRAMEBUFFER_COMPLETE) {
			Utils::LogFramebufferError(framebufferError);
			DeleteBuffer();
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void Framebuffer::DeleteBuffer()
	{
		if (!m_FramebufferID) return;

		glDeleteFramebuffers(1, &m_FramebufferID);
		for (const auto& id : m_ColorAttachments) { glDeleteTextures(1, &id); }

		glDeleteTextures(1, &m_DepthAttachment);
		glDeleteTextures(1, &m_ShadowMapAttachment);
	}

	void Framebuffer::AddShadowMapAttachment()
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &m_ShadowMapAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ShadowMapAttachment);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, m_Specs.Width, m_Specs.Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

		GLfloat bColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bColor);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		//glDrawBuffer(GL_NONE);
		//glReadBuffer(GL_NONE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMapAttachment, 0);
	}

	int Framebuffer::TextureFormatToInternalGL(const FramebufferTextureFormat format)
	{
		switch (format) {
			case FramebufferTextureFormat::RGBA8:
				return GL_RGBA8;
			case FramebufferTextureFormat::RGBA16:
				return GL_RGBA16;
			case FramebufferTextureFormat::RGBA16F:
				return GL_RGBA16F;
			case FramebufferTextureFormat::RGBA32F:
				return GL_RGBA32F;
			case FramebufferTextureFormat::RED_INTEGER:
				return GL_R32I;
			default:
				return GL_RGBA8;
		}
	}

	int Framebuffer::TextureFormatToGL(const FramebufferTextureFormat format)
	{
		switch (format) {
			case FramebufferTextureFormat::RED_INTEGER:
				return GL_RED_INTEGER;
			default:
				return GL_RGBA;
		}
	}

}
