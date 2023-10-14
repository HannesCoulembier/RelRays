#pragma once

#include "LoFox/Renderer/Pipeline.h"

#include <vulkan/vulkan.h>

namespace LoFox {

	struct VulkanGraphicsPipelineData {

		VkPipeline Pipeline = {};
	};

	class VulkanGraphicsPipeline : public GraphicsPipeline {

	public:
		VulkanGraphicsPipeline(GraphicsPipelineCreateInfo createInfo);

		virtual void Destroy() override;
	private:
		GraphicsPipelineCreateInfo m_CreateInfo = {};
		VulkanGraphicsPipelineData m_VulkanData = {};

		VkPipelineLayout m_Layout = {};

		VkPipelineVertexInputStateCreateInfo m_VertexInputStateCreateInfo = {};
		VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyStateCreateInfo = {};
		VkPipelineRasterizationStateCreateInfo m_RasterizationStateCreateInfo = {};
		VkPipelineMultisampleStateCreateInfo m_MultisampleStateCreateInfo = {};
		VkPipelineColorBlendAttachmentState m_ColorBlendAttachmentState = {};

		VkViewport m_Viewport;
		VkRect2D m_Scissor;
	};
}