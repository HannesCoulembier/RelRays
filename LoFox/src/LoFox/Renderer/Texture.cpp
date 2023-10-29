#include "lfpch.h"
#include "LoFox/Renderer/Texture.h"

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/OpenGLTexture.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/VulkanTexture.h"
#endif

namespace LoFox {

	Ref<Texture> Texture::Create() {

		#ifdef LF_RENDERAPI_OPENGL
			return CreateRef<OpenGLTexture>();
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			return CreateRef<VulkanTexture>();
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}
}