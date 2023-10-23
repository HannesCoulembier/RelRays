#include "lfpch.h"
#include "Platform/Vulkan/VulkanPipeline.h"

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanVertexBuffer.h"
#include "Platform/Vulkan/Utils.h"

namespace LoFox {

	VkPrimitiveTopology TopologyToVkPrimitiveTopology(Topology topology) {

		switch (topology) {
			case Topology::Triangle: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			case Topology::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			case Topology::Point: LF_CORE_ERROR("Vulkan Implementation does not support Topology::Point! see https://stackoverflow.com/questions/74979938/why-do-i-have-to-write-pointsize-in-the-vertex-shader for more information");
			// case Topology::Point: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
		}

		LF_CORE_ASSERT(false, "Topology not available!");
		return VkPrimitiveTopology(0);
	}

	VulkanGraphicsPipeline::VulkanGraphicsPipeline(GraphicsPipelineCreateInfo createInfo)
		: m_CreateInfo(createInfo) {

		m_Data = &m_VulkanData;

		// TODO: Make resource class or something to remove this code
		VkPipelineLayoutCreateInfo layoutCreateInfo = {};
		layoutCreateInfo.flags = 0;
		layoutCreateInfo.pNext = nullptr;
		layoutCreateInfo.pPushConstantRanges = nullptr; // We will not support push constants as OpenGL does not have those (use uniform buffer)
		layoutCreateInfo.pSetLayouts = nullptr;
		layoutCreateInfo.pushConstantRangeCount = 0; // We will not support push constants as OpenGL does not have those (use uniform buffer)
		layoutCreateInfo.setLayoutCount = 0;
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		LF_CORE_ASSERT(vkCreatePipelineLayout(VulkanContext::LogicalDevice, &layoutCreateInfo, nullptr, &m_Layout) == VK_SUCCESS, "Failed to create pipeline layout!");
		// END TODO

		std::vector<VkVertexInputBindingDescription> bindings = {
			Utils::VertexLayoutToVertexInputBindingDescription(0, m_CreateInfo.VertexLayout)
		};
		
		std::vector<VkVertexInputAttributeDescription> attributes;
		Utils::AddVertexInputAttributeDescriptionsFromVertexLayout(0, m_CreateInfo.VertexLayout, attributes);
		
		m_VertexInputStateCreateInfo = Utils::MakePipelineVertexInputStateCreateInfo(bindings, attributes);
		m_InputAssemblyStateCreateInfo = Utils::MakePipelineInputAssemblyStateCreateInfo(TopologyToVkPrimitiveTopology(m_CreateInfo.Topology));
		m_RasterizationStateCreateInfo = Utils::MakePipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL, m_CreateInfo.LineWidth);
		m_MultisampleStateCreateInfo = Utils::MakePipelineMultisampleStateCreateInfo(); // TODO: investigate adding more parameters to this function?
		m_ColorBlendAttachmentState = Utils::MakePipelineColorBlendAttachmentState(); // TODO: investigate adding more parameters to this function?

		// TODO: add user control over viewport(s) min and max depth, x-y offset and width and height (should be like OpenGL for user, so corrections we do here, stay here)
		m_Viewport.height = -(float)VulkanContext::SwapchainExtent.height; // Makes (-1, -1) left bottom, (1, 1) top right, like OpenGL does
		m_Viewport.maxDepth = 1.0f;
		m_Viewport.minDepth = 0.0f;
		m_Viewport.width = (float)VulkanContext::SwapchainExtent.width;
		m_Viewport.x = 0.0f;
		m_Viewport.y = (float)VulkanContext::SwapchainExtent.height; // Makes (-1, -1) left bottom, (1, 1) top right, like OpenGL does

		// TODO: investigate if this exists in OpenGL and if so, add user control over scissor(s)
		m_Scissor.extent = VulkanContext::SwapchainExtent;
		m_Scissor.offset = { 0, 0 };

		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.flags = 0;
		viewportState.pNext = nullptr;
		viewportState.pScissors = &m_Scissor;
		viewportState.pViewports = &m_Viewport;
		viewportState.scissorCount = 1;
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;

		// TODO: Investigate parameters of VkPipelineColorBlendStateCreateInfo. See if any are usefull for the user and exist in OpenGL
		VkPipelineColorBlendStateCreateInfo colorBlending = {};
		colorBlending.attachmentCount = 1;
		// colorBlending.blendConstants = 
		colorBlending.flags = 0;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.pAttachments = &m_ColorBlendAttachmentState;
		colorBlending.pNext = nullptr;
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

		VkShaderModule vertexModule = static_cast<VulkanShaderData*>(m_CreateInfo.VertexShader->GetData())->ShaderModule;
		VkShaderModule fragmentModule = static_cast<VulkanShaderData*>(m_CreateInfo.FragmentShader->GetData())->ShaderModule;

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {
			Utils::MakePipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, "main", vertexModule),
			Utils::MakePipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, "main", fragmentModule)
		};

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		// pipelineCreateInfo.basePipelineIndex = 
		pipelineCreateInfo.flags = 0;
		pipelineCreateInfo.layout = m_Layout;
		pipelineCreateInfo.pColorBlendState = &colorBlending;
		pipelineCreateInfo.pDepthStencilState = nullptr; // TODO: add depth and stencil buffers
		pipelineCreateInfo.pDynamicState = nullptr; // TODO: add dynamic states (where compatible with OpenGL)
		pipelineCreateInfo.pInputAssemblyState = &m_InputAssemblyStateCreateInfo;
		pipelineCreateInfo.pMultisampleState = &m_MultisampleStateCreateInfo;
		pipelineCreateInfo.pNext = nullptr;
		pipelineCreateInfo.pRasterizationState = &m_RasterizationStateCreateInfo;
		pipelineCreateInfo.pStages = shaderStages.data();
		pipelineCreateInfo.pTessellationState = nullptr; // TODO: investigate what this does and if we need to set it
		pipelineCreateInfo.pVertexInputState = &m_VertexInputStateCreateInfo;
		pipelineCreateInfo.pViewportState = &viewportState;
		pipelineCreateInfo.renderPass = VulkanContext::RenderPass;
		pipelineCreateInfo.stageCount = shaderStages.size();
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.subpass = 0;

		LF_CORE_ASSERT(vkCreateGraphicsPipelines(VulkanContext::LogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_VulkanData.Pipeline) == VK_SUCCESS, "Failed to create graphics pipeline!");

	}

	void VulkanGraphicsPipeline::Destroy() {

		// Wait for graphics queue to finish rendering before deleting the pipeline
		vkQueueWaitIdle(VulkanContext::GraphicsQueueHandle);

		vkDestroyPipeline(VulkanContext::LogicalDevice, m_VulkanData.Pipeline, nullptr);
		vkDestroyPipelineLayout(VulkanContext::LogicalDevice, m_Layout, nullptr);
	}
}