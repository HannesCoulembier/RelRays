#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

namespace LoFox {

	enum PipelineType {
		Graphics,
	};

	struct GraphicsPipelineCreateInfo {

		std::string VertexShaderPath = "";
		std::string FragmentShaderPath = "";

		std::vector<VkAttachmentDescription> Attachments;

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

		void InitLayout();
		void InitRenderPass();
		void InitPipeline();

		void Destroy();
	};

	class Pipeline {

	public:
		static GraphicsPipeline CreateGraphicsPipeline(GraphicsPipelineCreateInfo createInfo);
	};
}