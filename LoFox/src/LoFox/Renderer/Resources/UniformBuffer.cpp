#include "lfpch.h"
#include "LoFox/Renderer/Resources/UniformBuffer.h"

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/OpenGLUniformBuffer.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/Resources/VulkanUniformBuffer.h"
#endif

namespace LoFox {

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size) {

		#ifdef LF_RENDERAPI_OPENGL
			return CreateRef<OpenGLUniformBuffer>(size);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			return CreateRef<VulkanUniformBuffer>(size);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}
}