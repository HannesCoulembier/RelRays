#include "lfpch.h"
#include "LoFox/Renderer/Resources/Texture.h"

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/Resources/OpenGLTexture.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/Resources/VulkanTexture.h"
#endif

namespace LoFox {

	Ref<Texture> Texture::Create(const std::string& path) {

		#ifdef LF_RENDERAPI_OPENGL
				return CreateRef<OpenGLTexture>(path);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
				return CreateRef<VulkanTexture>(path);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	Texture::Texture(const std::string& path)
		: m_Path(path) {}
}