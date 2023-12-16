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

	void GraphicsContext::Init(Ref<Window> window) {

		#ifdef LF_RENDERAPI_OPENGL
			OpenGLContext::Init(window); return;
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			VulkanContext::Init(window); return;
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

	void GraphicsContext::BeginFrame() {

		#ifdef LF_RENDERAPI_OPENGL
			OpenGLContext::BeginFrame(); return;
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			VulkanContext::BeginFrame(); return;
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	void GraphicsContext::BeginFramebuffer(Ref<Framebuffer> framebuffer, glm::vec3 clearColor) {

		#ifdef LF_RENDERAPI_OPENGL
			OpenGLContext::BeginFramebuffer(framebuffer, clearColor); return;
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			VulkanContext::BeginFramebuffer(framebuffer, clearColor); return;
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	void GraphicsContext::SetActivePipeline(Ref<GraphicsPipeline> pipeline) {

		#ifdef LF_RENDERAPI_OPENGL
			OpenGLContext::SetActivePipeline(pipeline); return;
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			VulkanContext::SetActivePipeline(pipeline); return;
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	void GraphicsContext::Draw(Ref<IndexBuffer> indexBuffer, Ref<VertexBuffer> vertexBuffer) {

		#ifdef LF_RENDERAPI_OPENGL
			OpenGLContext::Draw(indexBuffer, vertexBuffer); return;
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			VulkanContext::Draw(indexBuffer, vertexBuffer); return;
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	void GraphicsContext::EndFramebuffer() {

		#ifdef LF_RENDERAPI_OPENGL
			OpenGLContext::EndFramebuffer(); return;
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			VulkanContext::EndFramebuffer(); return;
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

	void GraphicsContext::WaitIdle() {

		#ifdef LF_RENDERAPI_OPENGL
			OpenGLContext::WaitIdle(); return;
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			VulkanContext::WaitIdle(); return;
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

}