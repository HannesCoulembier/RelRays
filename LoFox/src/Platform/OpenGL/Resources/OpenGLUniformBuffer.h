#pragma once

#include "LoFox/Renderer/Resources/UniformBuffer.h"

#include <glad/glad.h>

namespace LoFox {

	struct OpenGLUniformBufferData {

		GLuint RendererID;
	};

	class OpenGLUniformBuffer : public UniformBuffer {

	public:
		OpenGLUniformBuffer(uint32_t size);
		virtual void Destroy() override;

		virtual void SetData(const void* data) override;
	private:
		OpenGLUniformBufferData m_OpenGLData;

		uint32_t m_Size;
	};
}
