#include "lfpch.h"
#include "LoFox/Renderer/Pipeline.h"

#include "LoFox/Renderer/Shader.h"

#include "LoFox/Renderer/RenderContext.h"

namespace LoFox {

	GraphicsPipeline Pipeline::CreateGraphicsPipeline(GraphicsPipelineCreateInfo createInfo) {

		GraphicsPipeline pipeline = {};

		pipeline.CreateInfo = createInfo;

		pipeline.InitLayout();
		pipeline.InitRenderPass();
		pipeline.InitPipeline();

		return pipeline;
	}

	void GraphicsPipeline::InitLayout() {

		VkPipelineLayoutCreateInfo layoutCreateInfo = {};
		// layoutCreateInfo.flags = 
		// layoutCreateInfo.pNext = 
		layoutCreateInfo.pPushConstantRanges = nullptr;
		layoutCreateInfo.pSetLayouts = &CreateInfo.DescriptorSetLayout;
		layoutCreateInfo.pushConstantRangeCount = 0;
		layoutCreateInfo.setLayoutCount = 1;
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		LF_CORE_ASSERT(vkCreatePipelineLayout(RenderContext::LogicalDevice, &layoutCreateInfo, nullptr, &Layout) == VK_SUCCESS, "Failed to create pipeline layout!");
	}

	void GraphicsPipeline::InitRenderPass() {

		VkSubpassDependency subpassDependency = {};
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.dstSubpass = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentRef;
		subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;

		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.attachmentCount = CreateInfo.Attachments.size();
		renderPassCreateInfo.dependencyCount = 1;
		// renderPassCreateInfo.flags = 
		renderPassCreateInfo.pAttachments = CreateInfo.Attachments.data();
		renderPassCreateInfo.pDependencies = &subpassDependency;
		// renderPassCreateInfo.pNext = 
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.subpassCount = 1;

		LF_CORE_ASSERT(vkCreateRenderPass(RenderContext::LogicalDevice, &renderPassCreateInfo, nullptr, &RenderPass) == VK_SUCCESS, "Failed to create render pass!");

	}

	void GraphicsPipeline::InitPipeline() {

		VkPipelineColorBlendAttachmentState colorBlendingAttachment = {};
		colorBlendingAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendingAttachment.blendEnable = VK_TRUE;
		colorBlendingAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendingAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendingAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendingAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendingAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendingAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlendingState = {};
		colorBlendingState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendingState.logicOpEnable = VK_FALSE;
		colorBlendingState.logicOp = VK_LOGIC_OP_COPY;
		colorBlendingState.attachmentCount = 1;
		colorBlendingState.pAttachments = &colorBlendingAttachment;
		colorBlendingState.blendConstants[0] = 0.0f;
		colorBlendingState.blendConstants[1] = 0.0f;
		colorBlendingState.blendConstants[2] = 0.0f;
		colorBlendingState.blendConstants[3] = 0.0f;

		VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.depthTestEnable = VK_TRUE;
		depthStencilState.depthWriteEnable = VK_TRUE;
		depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencilState.depthBoundsTestEnable = VK_FALSE;
		depthStencilState.minDepthBounds = 0.0f;
		depthStencilState.maxDepthBounds = 1.0f;
		depthStencilState.stencilTestEnable = VK_FALSE;
		depthStencilState.front = {};
		depthStencilState.back = {};

		std::vector<VkDynamicState> dynamicStates = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamicState = {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
		inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyState.primitiveRestartEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampleState = {};
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.sampleShadingEnable = VK_FALSE;
		multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleState.minSampleShading = 1.0f;
		multisampleState.pSampleMask = nullptr;
		multisampleState.alphaToCoverageEnable = VK_FALSE;
		multisampleState.alphaToOneEnable = VK_FALSE;

		VkPipelineRasterizationStateCreateInfo rasterizerState = {};
		rasterizerState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerState.depthClampEnable = VK_FALSE;
		rasterizerState.rasterizerDiscardEnable = VK_FALSE;
		rasterizerState.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerState.lineWidth = 1.0f;
		rasterizerState.cullMode = VK_CULL_MODE_NONE;
		rasterizerState.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerState.depthBiasEnable = VK_FALSE;
		rasterizerState.depthBiasConstantFactor = 0.0f;
		rasterizerState.depthBiasClamp = 0.0f;
		rasterizerState.depthBiasSlopeFactor = 0.0f;

		Shader vertexShader(CreateInfo.VertexShaderPath, ShaderType::Vertex);
		Shader fragmentShader(CreateInfo.FragmentShaderPath, ShaderType::Fragment);

		std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertexShader.GetCreateInfo(), fragmentShader.GetCreateInfo() };

		VkPipelineVertexInputStateCreateInfo vertexInputState = {};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputState.vertexBindingDescriptionCount = 1;
		vertexInputState.pVertexBindingDescriptions = &CreateInfo.VertexBindingDescription;
		vertexInputState.vertexAttributeDescriptionCount = (uint32_t)CreateInfo.VertexAttributeDescriptions.size();
		vertexInputState.pVertexAttributeDescriptions = CreateInfo.VertexAttributeDescriptions.data();

		VkPipelineViewportStateCreateInfo viewportState = {}; // Viewport and Scissors are dynamic states
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		// Creating the pipeline infostruct
		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex = -1;
		// pipelineCreateInfo.flags = 
		pipelineCreateInfo.layout = Layout;
		pipelineCreateInfo.pColorBlendState = &colorBlendingState;
		pipelineCreateInfo.pDepthStencilState = &depthStencilState;
		pipelineCreateInfo.pDynamicState = &dynamicState;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
		pipelineCreateInfo.pMultisampleState = &multisampleState;
		// pipelineCreateInfo.pNext = 
		pipelineCreateInfo.pRasterizationState = &rasterizerState;
		pipelineCreateInfo.pStages = shaderStages.data();
		// pipelineCreateInfo.pTessellationState = 
		pipelineCreateInfo.pVertexInputState = &vertexInputState;
		pipelineCreateInfo.pViewportState = &viewportState;
		pipelineCreateInfo.renderPass = RenderPass;
		pipelineCreateInfo.stageCount = (uint32_t)shaderStages.size();
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.subpass = 0;

		LF_CORE_ASSERT(vkCreateGraphicsPipelines(RenderContext::LogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &Pipeline) == VK_SUCCESS, "Failed to create graphics pipeline!");
	}

	void GraphicsPipeline::Destroy() {

		vkDestroyPipeline(RenderContext::LogicalDevice, Pipeline, nullptr);
		vkDestroyPipelineLayout(RenderContext::LogicalDevice, Layout, nullptr);
		vkDestroyRenderPass(RenderContext::LogicalDevice, RenderPass, nullptr);
	}
}