#include "lfpch.h"
#include "LoFox/Renderer/Pipeline.h"

#include "LoFox/Renderer/Shader.h"
#include "LoFox/Renderer/VertexBuffer.h"

#include "LoFox/Renderer/Renderer.h"
#include "LoFox/Renderer/RenderCommand.h"
#include "LoFox/Renderer/RenderContext.h"

namespace LoFox {

	ComputePipelineBuilder::ComputePipelineBuilder(ComputePipelineCreateInfo createInfo)
		: m_CreateInfo(createInfo) {

		m_Pipeline = CreateRef<ComputePipeline>();
		m_Pipeline->CreateInfo = m_CreateInfo;
	}

	Ref<ComputePipeline> ComputePipelineBuilder::CreateComputePipeline() {

		m_Pipeline->InitLayout();
		m_Pipeline->InitDescriptorSets();
		m_Pipeline->InitPipeline();

		return m_Pipeline;
	}

	void ComputePipelineBuilder::PreparePushConstant(uint32_t objectSize, VkShaderStageFlags shaderStage) {

		LF_CORE_ASSERT(!(m_Pipeline->m_PushConstantsStagesUsed & shaderStage), "There can only be one push constant per shader stage!");
		m_Pipeline->m_PushConstantsStagesUsed |= shaderStage;

		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.offset = m_Pipeline->m_PushConstantsTotalOffset;
		pushConstantRange.size = objectSize;
		pushConstantRange.stageFlags = shaderStage;

		m_Pipeline->m_PushConstants.push_back(pushConstantRange);
		m_Pipeline->m_PushConstantsData.push_back(nullptr);
		m_Pipeline->m_PushConstantsTotalOffset += objectSize;
	}

	void ComputePipeline::PushConstant(uint32_t index, const void* data) { m_PushConstantsData[index] = data; }

	void ComputePipeline::InitLayout() {

		VkDescriptorSetLayout descriptorSetLayout = CreateInfo.ResourceLayout->GetDescriptorSetLayout();

		VkPipelineLayoutCreateInfo layoutCreateInfo = {};
		// layoutCreateInfo.flags = 
		// layoutCreateInfo.pNext = 
		layoutCreateInfo.pPushConstantRanges = m_PushConstants.data();
		layoutCreateInfo.pSetLayouts = &descriptorSetLayout;
		layoutCreateInfo.pushConstantRangeCount = m_PushConstants.size();
		layoutCreateInfo.setLayoutCount = 1;
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		LF_CORE_ASSERT(vkCreatePipelineLayout(RenderContext::LogicalDevice, &layoutCreateInfo, nullptr, &Layout) == VK_SUCCESS, "Failed to create pipeline layout!");
	}

	void ComputePipeline::InitDescriptorSets() {

		CreateDescriptorPool();

		// Allocate Descriptor sets
		std::vector<VkDescriptorSetLayout> layouts(Renderer::MaxFramesInFlight, CreateInfo.ResourceLayout->GetDescriptorSetLayout());
		VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
		descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocInfo.descriptorPool = m_DescriptorPool;
		descriptorSetAllocInfo.descriptorSetCount = (uint32_t)(Renderer::MaxFramesInFlight);
		descriptorSetAllocInfo.pSetLayouts = layouts.data();

		m_DescriptorSets.resize(Renderer::MaxFramesInFlight);
		LF_CORE_ASSERT(vkAllocateDescriptorSets(RenderContext::LogicalDevice, &descriptorSetAllocInfo, m_DescriptorSets.data()) == VK_SUCCESS, "Failed to allocate descriptor sets!");

		// Update Descriptor sets
		for (size_t i = 0; i < Renderer::MaxFramesInFlight; i++) {

			std::vector<VkWriteDescriptorSet> descriptorWrites = {};
			uint32_t binding = 0;
			for (const auto& resource : CreateInfo.ResourceLayout->GetResources()) {

				VkDescriptorBufferInfo bufferInfo = {};
				if (resource.BufferDescriptorInfos.size() != 0)
					bufferInfo = resource.BufferDescriptorInfos[i];

				std::vector<VkDescriptorImageInfo> imageInfo = {};
				if (resource.ImageDescriptorInfos.size() != 0)
					imageInfo = resource.ImageDescriptorInfos;

				VkWriteDescriptorSet descriptorWrite = {};
				descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				descriptorWrite.dstSet = m_DescriptorSets[i];
				descriptorWrite.dstBinding = binding;
				descriptorWrite.dstArrayElement = 0;
				descriptorWrite.descriptorType = resource.Type;
				descriptorWrite.descriptorCount = resource.DescriptorCount;
				descriptorWrite.pBufferInfo = (resource.BufferDescriptorInfos.size() == 0) ? nullptr : &bufferInfo;
				descriptorWrite.pImageInfo = (resource.ImageDescriptorInfos.size() == 0) ? nullptr : imageInfo.data();
				descriptorWrite.pTexelBufferView = nullptr;

				descriptorWrites.push_back(descriptorWrite);
				binding++;

				vkUpdateDescriptorSets(RenderContext::LogicalDevice, 1, &descriptorWrite, 0, nullptr);
			}
		}
	}

	void ComputePipeline::InitPipeline() {


		Shader shader(CreateInfo.ComputeShaderPath, ShaderType::Compute);
		VkPipelineShaderStageCreateInfo shaderStage = shader.GetCreateInfo();

		VkComputePipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex = -1;
		// pipelineCreateInfo.flags = 
		pipelineCreateInfo.layout = Layout;
		// pipelineCreateInfo.pNext = 
		pipelineCreateInfo.stage = shaderStage;
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;

		VkResult r = vkCreateComputePipelines(RenderContext::LogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &Pipeline);
		LF_CORE_ASSERT(r == VK_SUCCESS, "Failed to create compute pipeline!");
	}

	void ComputePipeline::CreateDescriptorPool() {

		std::vector<VkDescriptorPoolSize> descriptorPoolSizes = {};
		for (const auto& resource : CreateInfo.ResourceLayout->GetResources()) {

			VkDescriptorPoolSize descriptorPoolSize = {};
			descriptorPoolSize.type = resource.Type;
			descriptorPoolSize.descriptorCount = (uint32_t)(Renderer::MaxFramesInFlight);
			descriptorPoolSizes.push_back(descriptorPoolSize);
		}

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.poolSizeCount = descriptorPoolSizes.size();
		descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
		descriptorPoolCreateInfo.maxSets = (uint32_t)(Renderer::MaxFramesInFlight);

		LF_CORE_ASSERT(vkCreateDescriptorPool(RenderContext::LogicalDevice, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool) == VK_SUCCESS, "Failed to create descriptor pool!");
	}

	void ComputePipeline::Destroy() {

		vkDestroyPipelineLayout(RenderContext::LogicalDevice, Layout, nullptr);
		vkDestroyPipeline(RenderContext::LogicalDevice, Pipeline, nullptr);
		vkDestroyDescriptorPool(RenderContext::LogicalDevice, m_DescriptorPool, nullptr);
	}

	void ComputePipeline::Bind() {

		VkCommandBuffer commandBuffer = Renderer::GetCommandBuffer();
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, Pipeline);

		// Bind descriptor sets
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, Layout, 0, 1, &m_DescriptorSets[Renderer::GetCurrentFrame()], 0, nullptr);
	}

	void ComputePipeline::Dispatch(uint32_t width, uint32_t height, uint32_t groupWidth, uint32_t groupHeight) {

		VkCommandBuffer commandBuffer = Renderer::GetCommandBuffer();

		for (size_t i = 0; i < m_PushConstants.size(); i++) {

			VkPushConstantRange pushConstant = m_PushConstants[i];
			const void* data = m_PushConstantsData[i];
			vkCmdPushConstants(commandBuffer, Layout, pushConstant.stageFlags, pushConstant.offset, pushConstant.size, data);
		}

		vkCmdDispatch(commandBuffer, (uint32_t)((float)width / (float)groupWidth),(uint32_t)((float)height / (float)groupHeight), 1);
	}

	GraphicsPipelineBuilder::GraphicsPipelineBuilder(GraphicsPipelineCreateInfo createInfo)
		: m_CreateInfo(createInfo) {

		m_Pipeline = CreateRef<GraphicsPipeline>();
		m_Pipeline->CreateInfo = m_CreateInfo;
	}

	Ref<GraphicsPipeline> GraphicsPipelineBuilder::CreateGraphicsPipeline() {

		m_Pipeline->InitLayout();
		m_Pipeline->InitRenderPass();
		m_Pipeline->InitPipeline();

		return m_Pipeline;
	}

	void GraphicsPipelineBuilder::PreparePushConstant(uint32_t objectSize, VkShaderStageFlags shaderStage) {

		LF_CORE_ASSERT(!(m_Pipeline->m_PushConstantsStagesUsed & shaderStage), "There can only be one push constant per shader stage!");
		m_Pipeline->m_PushConstantsStagesUsed |= shaderStage;

		VkPushConstantRange pushConstantRange = {};
		pushConstantRange.offset = m_Pipeline->m_PushConstantsTotalOffset;
		pushConstantRange.size = objectSize;
		pushConstantRange.stageFlags = shaderStage;

		m_Pipeline->m_PushConstants.push_back(pushConstantRange);
		m_Pipeline->m_PushConstantsData.push_back(nullptr);
		m_Pipeline->m_PushConstantsTotalOffset += objectSize;
	}

	void GraphicsPipelineBuilder::PrepareVertexBuffer(Ref<VertexBuffer> buffer) { m_Pipeline->m_VertexBuffer = buffer; }

	void GraphicsPipeline::PushConstant(uint32_t index, const void* data) { m_PushConstantsData[index] = data; }

	void GraphicsPipeline::InitLayout() {

		// std::vector<VkPushConstantRange> pushConstants = RenderCommand::GetPushConstants();

		VkDescriptorSetLayout descriptorSetLayout = CreateInfo.ResourceLayout->GetDescriptorSetLayout();
		VkPipelineLayoutCreateInfo layoutCreateInfo = {};
		// layoutCreateInfo.flags = 
		// layoutCreateInfo.pNext = 
		layoutCreateInfo.pPushConstantRanges = m_PushConstants.data();
		layoutCreateInfo.pSetLayouts = &descriptorSetLayout;
		layoutCreateInfo.pushConstantRangeCount = m_PushConstants.size();
		layoutCreateInfo.setLayoutCount = 1;
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

		LF_CORE_ASSERT(vkCreatePipelineLayout(RenderContext::LogicalDevice, &layoutCreateInfo, nullptr, &Layout) == VK_SUCCESS, "Failed to create pipeline layout!");
	}

	void GraphicsPipeline::InitRenderPass() {

		// The image of the SwapChain
		VkAttachmentDescription colorAttachmentDescription = {};
		colorAttachmentDescription.format = Renderer::GetSwapChainImageFormat();
		colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		// The image of the depth buffer
		VkAttachmentDescription depthAttachmentDescription = {};
		depthAttachmentDescription.format = Renderer::GetSwapChainDepthFormat();
		depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDependency subpassDependency = {};
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.dstSubpass = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentRef;
		subpassDescription.pDepthStencilAttachment = &depthAttachmentRef;

		std::vector<VkAttachmentDescription> attachments = { colorAttachmentDescription, depthAttachmentDescription };
		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.attachmentCount = attachments.size();
		renderPassCreateInfo.dependencyCount = 1;
		// renderPassCreateInfo.flags = 
		renderPassCreateInfo.pAttachments = attachments.data();
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

		std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions = m_VertexBuffer->GetVertexLayout().GetAttributeDescriptions();
		VkPipelineVertexInputStateCreateInfo vertexInputState = {};
		vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputState.vertexBindingDescriptionCount = 1;
		vertexInputState.pVertexBindingDescriptions = &m_VertexBuffer->GetVertexLayout().GetBindingDescription();
		vertexInputState.vertexAttributeDescriptionCount = (uint32_t)vertexAttributeDescriptions.size();
		vertexInputState.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();

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