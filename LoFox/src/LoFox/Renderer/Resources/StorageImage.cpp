#include "lfpch.h"
#include "LoFox/Renderer/Resources/StorageImage.h"

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/Resources/OpenGLStorageImage.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/Resources/VulkanStorageImage.h"
#endif

namespace LoFox {

	Ref<StorageImage> StorageImage::Create(uint32_t width, uint32_t height) {

		#ifdef LF_RENDERAPI_OPENGL
				return CreateRef<OpenGLStorageImage>(width, height);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
				return CreateRef<VulkanStorageImage>(width, height);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	StorageImage::StorageImage(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height) {}
}