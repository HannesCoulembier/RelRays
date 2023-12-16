#pragma once
#include "LoFox/Renderer/Framebuffer.h"

#include <glad/glad.h>

namespace LoFox {

	struct OpenGLFramebufferData {

		GLuint RendererID = 0;
		std::vector<GLuint> Attachments;
	};

	class OpenGLFramebuffer : public Framebuffer {

	public:
		OpenGLFramebuffer(FramebufferCreateInfo createInfo);

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual void Destroy() override;

		virtual void* GetAttachmentImTextureID(uint32_t index) override;
	private:
		void Recreate();
	private:
		OpenGLFramebufferData m_OpenGLData;
		std::vector<FramebufferTextureDescription> m_AttachmentDescriptions;
	};
}
