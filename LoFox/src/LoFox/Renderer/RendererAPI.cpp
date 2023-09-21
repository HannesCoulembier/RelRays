#include "lfpch.h"
#include "LoFox/Renderer/RendererAPI.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace LoFox {

	// RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;

	Scope<RendererAPI> RendererAPI::Create() {

		switch (s_API) {

		case RendererAPI::API::None:	LF_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
		case RendererAPI::API::Vulkan:	return CreateScope<VulkanRendererAPI>();
		case RendererAPI::API::OpenGL:	return CreateScope<OpenGLRendererAPI>();
		}

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}