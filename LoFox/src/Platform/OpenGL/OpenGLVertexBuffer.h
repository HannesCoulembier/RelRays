#pragma once

#include "LoFox/Renderer/VertexBuffer.h"

#include <glad/glad.h>

namespace LoFox {

	GLenum VertexAttributeToOpenGLBaseType(VertexAttributeType attribute);

	struct OpenGLVertexBufferData {

		GLuint Buffer;
		VertexLayout VertexLayout = {};
	};

	class OpenGLVertexBuffer : public VertexBuffer {

	public:
		OpenGLVertexBuffer(uint32_t bufferSize, const void* data, VertexLayout layout);

		virtual void Destroy() override;
	private:
		OpenGLVertexBufferData m_OpenGLData;
	};
}
