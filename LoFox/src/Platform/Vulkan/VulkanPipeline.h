#pragma once
#include "LoFox/Renderer/Pipeline.h"

#include <vulkan/vulkan.h>

namespace LoFox {

	struct VulkanGraphicsPipelineData {

		VkPipeline Pipeline = {};
		VkPipelineLayout Layout = {};
		VkDescriptorSet DescriptorSet;
	};

	class VulkanGraphicsPipeline : public GraphicsPipeline {

	public:
		VulkanGraphicsPipeline(GraphicsPipelineCreateInfo createInfo);

		virtual void Destroy() override;
	private:
		GraphicsPipelineCreateInfo m_CreateInfo = {};
		VulkanGraphicsPipelineData m_VulkanData = {};

		VkPipelineVertexInputStateCreateInfo m_VertexInputStateCreateInfo = {};
		VkPipelineInputAssemblyStateCreateInfo m_InputAssemblyStateCreateInfo = {};
		VkPipelineRasterizationStateCreateInfo m_RasterizationStateCreateInfo = {};
		VkPipelineMultisampleStateCreateInfo m_MultisampleStateCreateInfo = {};
		VkPipelineColorBlendAttachmentState m_ColorBlendAttachmentState = {};
	};

	struct VulkanComputePipelineData {

		VkPipeline Pipeline = {};
		VkPipelineLayout Layout = {};
		VkDescriptorSet DescriptorSet;
	};

	class VulkanComputePipeline : public ComputePipeline {

	public:
		VulkanComputePipeline(ComputePipelineCreateInfo createInfo);

		virtual void Destroy() override;

		virtual void Dispatch(uint32_t width, uint32_t height, uint32_t groupWidth, uint32_t groupHeight) override;
	private:
		ComputePipelineCreateInfo m_CreateInfo = {};
		VulkanComputePipelineData m_VulkanData = {};
		VkCommandBuffer m_CommandBuffer = {};
	};
}