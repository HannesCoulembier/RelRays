#include "lfpch.h"
#include "LoFox/Renderer/Framebuffer.h"

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/OpenGLFramebuffer.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/VulkanFramebuffer.h"
#endif

namespace LoFox {

	Ref<Framebuffer> Framebuffer::Create(FramebufferCreateInfo createInfo) {

		#ifdef LF_RENDERAPI_OPENGL
			return CreateRef<OpenGLFramebuffer>(createInfo);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			return CreateRef<VulkanFramebuffer>(createInfo);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	Framebuffer::Framebuffer(FramebufferCreateInfo createInfo)
		: m_CreateInfo(createInfo) {}
}