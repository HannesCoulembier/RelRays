#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

namespace LoFox {

	struct testObject {
		glm::mat4 model;
	};

	enum PipelineType {
		Graphics,
	};

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

		void InitLayout();
		void InitRenderPass();
		void InitPipeline();

		void Destroy();
	};

	class GraphicsPipelineBuilder {

	public:
		GraphicsPipelineBuilder(GraphicsPipelineCreateInfo createInfo);
		GraphicsPipeline CreateGraphicsPipeline();
	private:
		GraphicsPipelineCreateInfo m_CreateInfo;
		GraphicsPipeline m_Pipeline;
	};
}