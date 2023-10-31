#pragma once
#include "LoFox/Renderer/Resources/StorageImage.h"

#include <glad/glad.h>

namespace LoFox {

	struct OpenGLStorageImageData {

		uint32_t RendererID;
	};

	class OpenGLStorageImage : public StorageImage {

	public:
		OpenGLStorageImage(uint32_t width, uint32_t height);
		virtual void Destroy() override;
	private:
		OpenGLStorageImageData m_OpenGLData;
	};
}