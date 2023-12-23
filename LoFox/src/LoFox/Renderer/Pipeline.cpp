#include "lfpch.h"
#include "LoFox/Renderer/Pipeline.h"

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/OpenGLPipeline.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/VulkanPipeline.h"
#endif

namespace LoFox {

	Ref<GraphicsPipeline> GraphicsPipeline::Create(GraphicsPipelineCreateInfo createInfo) {

		#ifdef LF_RENDERAPI_OPENGL
			return CreateRef<OpenGLGraphicsPipeline>(createInfo);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			return CreateRef<VulkanGraphicsPipeline>(createInfo);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	Ref<ComputePipeline> ComputePipeline::Create(ComputePipelineCreateInfo createInfo) {

		#ifdef LF_RENDERAPI_OPENGL
			return CreateRef<OpenGLComputePipeline>(createInfo);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			return CreateRef<VulkanComputePipeline>(createInfo);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}
}