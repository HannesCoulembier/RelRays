#include "lfpch.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"

#include "LoFox/Renderer/VertexBuffer.h"

namespace LoFox {

	GLenum VertexAttributeToOpenGLBaseType(VertexAttributeType attribute) {

		switch (attribute) {

			case VertexAttributeType::Float:	return GL_FLOAT;
			case VertexAttributeType::Float2:	return GL_FLOAT;
			case VertexAttributeType::Float3:	return GL_FLOAT;
			case VertexAttributeType::Float4:	return GL_FLOAT;

			case VertexAttributeType::Mat3:		return GL_FLOAT;
			case VertexAttributeType::Mat4:		return GL_FLOAT;

			case VertexAttributeType::Int:		return GL_INT;
			case VertexAttributeType::Int2:		return GL_INT;
			case VertexAttributeType::Int3:		return GL_INT;
			case VertexAttributeType::Int4:		return GL_INT;

			case VertexAttributeType::Uint:		return GL_UNSIGNED_INT;
			case VertexAttributeType::Bool:		return GL_BOOL;
		}

		LF_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(uint32_t bufferSize, const void* data, VertexLayout layout)
		: VertexBuffer(bufferSize, layout) {

		m_Data = &m_OpenGLData;
		m_OpenGLData.VertexLayout = layout;

		glGenBuffers(1, &m_OpenGLData.Buffer);
		glBindBuffer(GL_ARRAY_BUFFER, m_OpenGLData.Buffer);
		glBufferData(GL_ARRAY_BUFFER, bufferSize, data, GL_STATIC_DRAW);
	}

	void OpenGLVertexBuffer::Destroy() {

		glDeleteBuffers(1, &m_OpenGLData.Buffer);
	}
}