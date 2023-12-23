#include "lfpch.h"
#include "Platform/OpenGL/Resources/OpenGLStorageImage.h"

#include <stb_image.h>

namespace LoFox {

	OpenGLStorageImage::OpenGLStorageImage(uint32_t width, uint32_t height)
		: StorageImage(width, height) {

		m_Data = &m_OpenGLData;

		glGenTextures(1, &m_OpenGLData.RendererID);
		glBindTexture(GL_TEXTURE_2D, m_OpenGLData.RendererID);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_Width, m_Height);
	}

	void OpenGLStorageImage::Destroy() {

		glDeleteTextures(1, &m_OpenGLData.RendererID);
	}

	void* OpenGLStorageImage::GetImTextureID() {

		return &m_OpenGLData.RendererID;
	}
}