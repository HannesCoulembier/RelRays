#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

namespace LoFox {

	struct GraphicsPipelineCreateInfo {

		std::string VertexShaderPath = "";
		std::string FragmentShaderPath = "";

		VkVertexInputBindingDescription VertexBindingDescription;
		std::vector<VkVertexInputAttributeDescription> VertexAttributeDescriptions;

		VkDescriptorSetLayout DescriptorSetLayout;
	};

	class GraphicsPipeline {

	public:
		GraphicsPipelineCreateInfo CreateInfo;
		VkPipeline Pipeline;
		VkRenderPass RenderPass;
		VkPipelineLayout Layout;

		void PushConstant(uint32_t index, const void* data);

		void Destroy();

		std::vector<VkPushConstantRange> GetPushConstants() { return m_PushConstants; }
		std::vector<const void*> GetPushConstantsData() { return m_PushConstantsData; }
	private:
		void InitLayout();
		void InitRenderPass();
		void InitPipeline();
	private:
		uint32_t m_PushConstantsTotalOffset;
		VkShaderStageFlags m_PushConstantsStagesUsed;
		std::vector<VkPushConstantRange> m_PushConstants;
		std::vector<const void*> m_PushConstantsData;

		friend class GraphicsPipelineBuilder;
	};

	class GraphicsPipelineBuilder {

	public:
		GraphicsPipelineBuilder(GraphicsPipelineCreateInfo createInfo);
		Ref<GraphicsPipeline> CreateGraphicsPipeline();

		void PreparePushConstant(uint32_t objectSize, VkShaderStageFlags shaderStage);
	private:
		GraphicsPipelineCreateInfo m_CreateInfo;
		Ref<GraphicsPipeline> m_Pipeline;
	};
}