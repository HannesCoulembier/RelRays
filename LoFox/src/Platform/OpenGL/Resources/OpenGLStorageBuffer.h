#pragma once

#include "LoFox/Renderer/Resources/StorageBuffer.h"

#include <glad/glad.h>

namespace LoFox {

	struct OpenGLStorageBufferData {

		GLuint RendererID;
	};

	class OpenGLStorageBuffer : public StorageBuffer {

	public:
		OpenGLStorageBuffer(uint32_t maxObjectCount, uint32_t objectSize);
		virtual void Destroy() override;

		virtual void SetData(uint32_t objectCount, const void* data) override;
	private:
		OpenGLStorageBufferData m_OpenGLData;
	};
}
