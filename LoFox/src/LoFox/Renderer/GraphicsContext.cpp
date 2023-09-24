#include "lfpch.h"
#include "LoFox/Renderer/GraphicsContext.h"

#include "LoFox/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace LoFox {

	void GraphicsContext::Init(void* window) {

		switch (Renderer::GetAPI()) {

			case RendererAPI::API::None:	LF_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return;
			case RendererAPI::API::OpenGL:	OpenGLContext::Init(static_cast<GLFWwindow*>(window)); return;
			case RendererAPI::API::Vulkan:	VulkanContext::Init(static_cast<GLFWwindow*>(window)); return;
		}

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	void GraphicsContext::Shutdown() {

		switch (Renderer::GetAPI()) {

			case RendererAPI::API::None:	LF_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return;
			case RendererAPI::API::OpenGL:	OpenGLContext::Shutdown(); return;
			case RendererAPI::API::Vulkan:	VulkanContext::Shutdown(); return;
		}

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

}