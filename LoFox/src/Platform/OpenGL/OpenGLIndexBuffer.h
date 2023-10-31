#pragma once
#include "LoFox/Renderer/IndexBuffer.h"

#include <glad/glad.h>

namespace LoFox {

	struct OpenGLIndexBufferData {

		GLuint Buffer;
	};

	class OpenGLIndexBuffer : public IndexBuffer {

	public:
		OpenGLIndexBuffer(uint32_t numberOfIndices, const uint32_t* data);

		virtual void Destroy() override;
	private:
		OpenGLIndexBufferData m_OpenGLData;
	};
}
