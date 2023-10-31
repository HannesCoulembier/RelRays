#include "lfpch.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"

namespace LoFox {

	OpenGLIndexBuffer::OpenGLIndexBuffer(uint32_t numberOfIndices, const uint32_t* data)
		: IndexBuffer(numberOfIndices, data) {

		m_Data = &m_OpenGLData;

		glGenBuffers(1, &m_OpenGLData.Buffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_OpenGLData.Buffer);
		glBufferData(GL_ARRAY_BUFFER, m_Size, data, GL_STATIC_DRAW);
	}

	void OpenGLIndexBuffer::Destroy() {

		glDeleteBuffers(1, &m_OpenGLData.Buffer);
	}
}