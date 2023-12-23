#include "lfpch.h"
#include "LoFox/Renderer/RendererAPI.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace LoFox {

	#ifdef LF_RENDERAPI_OPENGL
		RendererAPI::API RendererAPI::s_API = RendererAPI::API::OpenGL;
	#else
#	ifdef LF_RENDERAPI_VULKAN
		RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;
	#else
		#error "LoFox needs a render api! f.e. define LF_RENDERAPI_OPENGL"
	#endif
	#endif

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