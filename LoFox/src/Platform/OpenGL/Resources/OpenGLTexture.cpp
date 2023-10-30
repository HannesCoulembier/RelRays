#include "lfpch.h"
#include "Platform/OpenGL/Resources/OpenGLTexture.h"

#include <stb_image.h>

namespace LoFox {

	OpenGLTexture::OpenGLTexture(const std::string& path)
		: Texture(path) {

		m_Data = &m_OpenGLData;

		m_InternalFormat = GL_SRGB_ALPHA;
		m_DataFormat = GL_RGBA;

		int width, height, channels;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* pixels = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		m_Width = width;
		m_Height = height;

		glGenTextures(1, &m_OpenGLData.RendererID);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_OpenGLData.RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); // TODO: give user control over GL_LINEAR (like with vulkan)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // TODO: give user control over GL_REPEAT (like with vulkan)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, m_InternalFormat, m_Width, m_Height, 0, m_DataFormat, GL_UNSIGNED_BYTE, pixels);

		stbi_image_free(pixels);
	}

	void OpenGLTexture::Destroy() {

		glDeleteTextures(1, &m_OpenGLData.RendererID);
	}
}