#include "lfpch.h"
#include "LoFox/Renderer/GraphicsContext.h"

#include "LoFox/Renderer/Renderer.h"
#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/OpenGLContext.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/VulkanContext.h"
#endif

namespace LoFox {

	void GraphicsContext::Init(void* window) {

		#ifdef LF_RENDERAPI_OPENGL
			OpenGLContext::Init(static_cast<GLFWwindow*>(window)); return;
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			VulkanContext::Init(static_cast<GLFWwindow*>(window)); return;
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	void GraphicsContext::Shutdown() {

		#ifdef LF_RENDERAPI_OPENGL
			OpenGLContext::Shutdown(); return;
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			VulkanContext::Shutdown(); return;
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	void GraphicsContext::BeginFrame(glm::vec3 clearColor) {

		#ifdef LF_RENDERAPI_OPENGL
			OpenGLContext::BeginFrame(clearColor); return;
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			VulkanContext::BeginFrame(clearColor); return;
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	void GraphicsContext::EndFrame() {

		#ifdef LF_RENDERAPI_OPENGL
			OpenGLContext::EndFrame(); return;
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			VulkanContext::EndFrame(); return;
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}
	
	void GraphicsContext::PresentFrame() {

		#ifdef LF_RENDERAPI_OPENGL
			OpenGLContext::PresentFrame(); return;
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			VulkanContext::PresentFrame(); return;
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

}