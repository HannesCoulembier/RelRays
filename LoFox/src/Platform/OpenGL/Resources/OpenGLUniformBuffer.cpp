#include "lfpch.h"
#include "Platform/OpenGL/Resources/OpenGLUniformBuffer.h"

namespace LoFox {

	OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size)
		: m_Size(size) {

		m_Data = &m_OpenGLData;

		glGenBuffers(1, &m_OpenGLData.RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_OpenGLData.RendererID);
		glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW); // TODO: investigate usage hint
	}

	void OpenGLUniformBuffer::Destroy() {

		glDeleteBuffers(1, &m_OpenGLData.RendererID);
	}

	void OpenGLUniformBuffer::SetData(const void* data) {

		glBindBuffer(GL_UNIFORM_BUFFER, m_OpenGLData.RendererID);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, m_Size, data);
	}
}