#include "lfpch.h"
#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace LoFox {

	OpenGLFramebuffer::OpenGLFramebuffer(FramebufferCreateInfo createInfo)
		: Framebuffer(createInfo) {

		m_Data = &m_OpenGLData;
		m_AttachmentDescriptions = m_CreateInfo.Attachments;

		Recreate();
	}

	void OpenGLFramebuffer::Recreate() {

		if (m_OpenGLData.RendererID != 0) // Start from scratch
			Destroy();

		glGenFramebuffers(1, &m_OpenGLData.RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_OpenGLData.RendererID);

		// Attachments
		if (m_AttachmentDescriptions.size()) {

			m_OpenGLData.Attachments.resize(m_AttachmentDescriptions.size());
			glGenTextures(m_OpenGLData.Attachments.size(), m_OpenGLData.Attachments.data());

			for (size_t i = 0; i < m_OpenGLData.Attachments.size(); i++) {

				glBindTexture(GL_TEXTURE_2D, m_OpenGLData.Attachments[i]);
				switch (m_AttachmentDescriptions[i].TextureFormat) {

					case FramebufferTextureFormat::RGBA8: {

						glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_CreateInfo.Width, m_CreateInfo.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

						glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_OpenGLData.Attachments[i], 0);
						break;
					}
					default: LF_CORE_ASSERT(false, "Unknown FramebufferTextureFormat!");
				}
			}
		}

		if (m_OpenGLData.Attachments.size() > 1) {

			LF_CORE_ASSERT(m_OpenGLData.Attachments.size() <= 32); // OpenGL only supports a maximum of 32 color attachments
			std::vector<GLenum> buffers;
			for (int i = 0; i < m_OpenGLData.Attachments.size(); i++)
				buffers.push_back(GL_COLOR_ATTACHMENT0 + i);
			glDrawBuffers(m_OpenGLData.Attachments.size(), buffers.data());
		}
		else if (m_OpenGLData.Attachments.empty()) {

			// Only depth-pass
			glDrawBuffer(GL_NONE);
		}

		LF_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void* OpenGLFramebuffer::GetAttachmentImTextureID(uint32_t index) {
		
		return &m_OpenGLData.Attachments[index];
	}

	void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height) {

		if (width == 0 || height == 0) {

			LF_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
			return;
		}

		m_CreateInfo.Width = width;
		m_CreateInfo.Height = height;

		Recreate();
	}

	void OpenGLFramebuffer::Destroy() {

		glDeleteFramebuffers(1, &m_OpenGLData.RendererID);
		glDeleteTextures(m_OpenGLData.Attachments.size(), m_OpenGLData.Attachments.data());

		m_OpenGLData.Attachments.clear();
		m_OpenGLData.RendererID = 0;
	}
}