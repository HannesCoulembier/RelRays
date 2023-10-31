#include "lfpch.h"
#include "Platform/OpenGL/Resources/OpenGLStorageBuffer.h"

namespace LoFox {

	OpenGLStorageBuffer::OpenGLStorageBuffer(uint32_t maxObjectCount, uint32_t objectSize)
		: StorageBuffer(maxObjectCount, objectSize) {

		m_Data = &m_OpenGLData;

		glGenBuffers(1, &m_OpenGLData.RendererID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_OpenGLData.RendererID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, m_Size, nullptr, GL_DYNAMIC_DRAW);
	}

	void OpenGLStorageBuffer::Destroy() {

		glDeleteBuffers(1, &m_OpenGLData.RendererID);
	}

	void OpenGLStorageBuffer::SetData(uint32_t objectCount, const void* data) {

		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, objectCount*m_ObjectSize, data);
	}
}