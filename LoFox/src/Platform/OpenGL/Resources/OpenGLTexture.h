#pragma once
#include "LoFox/Renderer/Resources/Texture.h"

#include <glad/glad.h>

namespace LoFox {

	struct OpenGLTextureData {

		uint32_t RendererID;
	};

	class OpenGLTexture : public Texture {

	public:
		OpenGLTexture(const std::string& path);
		virtual void Destroy() override;
	private:
		OpenGLTextureData m_OpenGLData;
		GLenum m_InternalFormat, m_DataFormat;
	};
}