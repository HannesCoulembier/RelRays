#include "lfpch.h"
#include "LoFox/Renderer/RenderContext.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "LoFox/Renderer/Shader.h"
#include "LoFox/Renderer/Buffer.h"
#include "LoFox/Renderer/SwapChain.h"
#include "LoFox/Renderer/Image.h"
#include "LoFox/Renderer/DebugMessenger.h"

#include "LoFox/Core/Settings.h"
#include "LoFox/Utils/VulkanUtils.h"
#include "LoFox/Utils/Utils.h"

#include "LoFox/Events/RenderEvent.h"

#include "LoFox/Renderer/Renderer.h"

#include "LoFox/Core/Application.h"

struct Vertex {

	glm::vec2 Position;
	glm::vec3 Color;
	glm::vec2 TexCoord;

	static VkVertexInputBindingDescription GetBindingDescription() {

		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions() {

		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, Color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, TexCoord);

		return attributeDescriptions;
	}
};

struct UniformBufferObject {

	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
};

const std::vector<uint32_t> vertexIndices = {
	0, 1, 2, 2, 3, 0
};

namespace LoFox {

	Ref<RenderContext> RenderContext::Create() {

		Ref<RenderContext> context = CreateRef<RenderContext>();
		context->LinkReference(context);
		return context;
	}

	void RenderContext::Init(Ref<Window> window) {

		m_Timer.Reset();

		m_Window = window;

		LF_OVERSPECIFY("Creating Vulkan instance");
		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanExtensions();
		Utils::ListAvailableVulkanLayers();
		#endif

		m_DebugMessenger = DebugMessenger::Create(m_ThisContext);

		InitInstance();

		m_DebugMessenger->Init();

		// Setting up Surface
		m_Window->CreateVulkanSurface(Instance, &Surface);

		// Pick physical device (= GPU to use)
		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanPhysicalDevices(Instance);
		#endif

		PhysicalDevice = Utils::PickVulkanPhysicalDevice(Instance, Surface);

		// Create logical device
		// Figuring out what queues we want
		Utils::QueueFamilyIndices indices = Utils::IdentifyVulkanQueueFamilies(PhysicalDevice, Surface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamilyIndex, indices.PresentFamilyIndex };
		float queuePriority = 1.0f;
		for (auto queueFamily : uniqueQueueFamilies) {

			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Actually setting up the logical device info structs
		VkPhysicalDeviceFeatures logicalDeviceFeatures = {}; // No features needed for now
		logicalDeviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo logicalDeviceCreateInfo{};
		logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		logicalDeviceCreateInfo.pEnabledFeatures = &logicalDeviceFeatures;
		logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(Utils::requiredVulkanDeviceExtensions.size());
		logicalDeviceCreateInfo.ppEnabledExtensionNames = Utils::requiredVulkanDeviceExtensions.data();
		logicalDeviceCreateInfo.enabledLayerCount = (uint32_t)(DebugMessenger::ValidationLayers.size());
		logicalDeviceCreateInfo.ppEnabledLayerNames = DebugMessenger::ValidationLayers.data();
		logicalDeviceCreateInfo.pNext = nullptr;

		LF_CORE_ASSERT(vkCreateDevice(PhysicalDevice, &logicalDeviceCreateInfo, nullptr, &LogicalDevice) == VK_SUCCESS, "Failed to create logical device!");

		// Retrieve queue handles
		vkGetDeviceQueue(LogicalDevice, indices.GraphicsFamilyIndex, 0, &GraphicsQueueHandle);
		vkGetDeviceQueue(LogicalDevice, indices.PresentFamilyIndex, 0, &PresentQueueHandle);

		m_SwapChain = CreateRef<SwapChain>(m_ThisContext, m_Window);

		// Create Render pass
		VkAttachmentDescription colorAttachmentDescription = {};
		colorAttachmentDescription.format = m_SwapChain->GetImageFormat();
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

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency subpassDependency = {};
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.dstSubpass = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.srcAccessMask = 0;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = 1;
		renderPassCreateInfo.pAttachments = &colorAttachmentDescription;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &subpassDependency;

		LF_CORE_ASSERT(vkCreateRenderPass(LogicalDevice, &renderPassCreateInfo, nullptr, &Renderpass) == VK_SUCCESS, "Failed to create render pass!");

		m_SwapChain->CreateFramebuffers();

		// Create Descriptorset layouts
		VkDescriptorSetLayoutBinding uboLayoutBinding = {}; // uniform buffer with MVP matrices
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutCreateInfo.bindingCount = (uint32_t)bindings.size();
		layoutCreateInfo.pBindings = bindings.data();

		LF_CORE_ASSERT(vkCreateDescriptorSetLayout(LogicalDevice, &layoutCreateInfo, nullptr, &m_DescriptorSetLayout) == VK_SUCCESS, "Failed to create descriptor set layout!");

		// Create Graphics pipeline
		Shader vertexShader(m_ThisContext, "Assets/Shaders/VertexShader.vert", ShaderType::Vertex);
		Shader fragmentShader(m_ThisContext, "Assets/Shaders/FragmentShader.frag", ShaderType::Fragment);

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShader.GetCreateInfo(), fragmentShader.GetCreateInfo() };

		auto bindingDescription = Vertex::GetBindingDescription();
		auto attributeDescriptions = Vertex::GetAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
		vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
		vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputCreateInfo.vertexAttributeDescriptionCount = (uint32_t)attributeDescriptions.size();
		vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
		inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

		VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {}; // Viewport and Scissors are dynamic states
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.viewportCount = 1;
		viewportStateCreateInfo.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
		rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerCreateInfo.depthClampEnable = VK_FALSE;
		rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerCreateInfo.lineWidth = 1.0f;
		rasterizerCreateInfo.cullMode = VK_CULL_MODE_NONE;
		rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
		rasterizerCreateInfo.depthBiasClamp = 0.0f;
		rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

		VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = {};
		multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
		multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisamplingCreateInfo.minSampleShading = 1.0f;
		multisamplingCreateInfo.pSampleMask = nullptr;
		multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
		multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachmentCreateInfo = {};
		colorBlendAttachmentCreateInfo.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachmentCreateInfo.blendEnable = VK_FALSE;
		colorBlendAttachmentCreateInfo.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentCreateInfo.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentCreateInfo.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentCreateInfo.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentCreateInfo.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachmentCreateInfo.alphaBlendOp = VK_BLEND_OP_ADD;

		VkPipelineColorBlendStateCreateInfo colorBlendingCreateInfo = {};
		colorBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendingCreateInfo.logicOpEnable = VK_FALSE;
		colorBlendingCreateInfo.logicOp = VK_LOGIC_OP_COPY;
		colorBlendingCreateInfo.attachmentCount = 1;
		colorBlendingCreateInfo.pAttachments = &colorBlendAttachmentCreateInfo;
		colorBlendingCreateInfo.blendConstants[0] = 0.0f;
		colorBlendingCreateInfo.blendConstants[1] = 0.0f;
		colorBlendingCreateInfo.blendConstants[2] = 0.0f;
		colorBlendingCreateInfo.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &m_DescriptorSetLayout;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		LF_CORE_ASSERT(vkCreatePipelineLayout(LogicalDevice, &pipelineLayoutCreateInfo, nullptr, &PipelineLayout) == VK_SUCCESS, "failed to create pipeline layout!");

		VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
		pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineCreateInfo.stageCount = 2;
		pipelineCreateInfo.pStages = shaderStages;
		pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
		pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
		pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
		pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
		pipelineCreateInfo.pDepthStencilState = nullptr;
		pipelineCreateInfo.pColorBlendState = &colorBlendingCreateInfo;
		pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		pipelineCreateInfo.layout = PipelineLayout;
		pipelineCreateInfo.renderPass = Renderpass;
		pipelineCreateInfo.subpass = 0;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex = -1;

		LF_CORE_ASSERT(vkCreateGraphicsPipelines(LogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &GraphicsPipeline) == VK_SUCCESS, "Failed to create graphics pipeline!");

		// Create Commandpool
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = indices.GraphicsFamilyIndex;

		LF_CORE_ASSERT(vkCreateCommandPool(LogicalDevice, &commandPoolCreateInfo, nullptr, &m_CommandPool) == VK_SUCCESS, "Failed to create command pool!");

		// Create textures
		Image1 = CreateRef<Image>(m_ThisContext, "Assets/Textures/Rick.png");

		CreateImageSampler();

		// Create Vertex buffer
		{ // This is a scope to ensure the destructor of vertexStagingBuffer is called
			uint32_t vertexBufferSize = sizeof(vertices[0]) * vertices.size();
			Ref<Buffer> vertexStagingBuffer = CreateRef<Buffer>(m_ThisContext, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			VertexBuffer = CreateRef<Buffer>(m_ThisContext, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			vertexStagingBuffer->SetData(vertices.data());

			CopyBuffer(vertexStagingBuffer, VertexBuffer);
		}
		// Create Index buffer
		{
			uint32_t indexBufferSize = sizeof(vertexIndices[0]) * vertexIndices.size();
			Ref<Buffer> indexStagingBuffer = CreateRef<Buffer>(m_ThisContext, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			IndexBuffer = CreateRef<Buffer>(m_ThisContext, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			indexStagingBuffer->SetData(vertexIndices.data());

			CopyBuffer(indexStagingBuffer, IndexBuffer);
		}
		// Create UniformBuffers
		{
			uint32_t uniformBufferSize = sizeof(UniformBufferObject);
			m_UniformBuffers.resize(m_MaxFramesInFlight);
			m_UniformBuffersMapped.resize(m_MaxFramesInFlight);

			for (size_t i = 0; i < m_MaxFramesInFlight; i++) {

				m_UniformBuffers[i] = CreateRef<Buffer>(m_ThisContext, uniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

				vkMapMemory(LogicalDevice, m_UniformBuffers[i]->GetMemory(), 0, uniformBufferSize, 0, &m_UniformBuffersMapped[i]);
			}
		}

		// Create Descriptor pool
		std::array<VkDescriptorPoolSize, 2> descriptorPoolSizes = {};
		descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorPoolSizes[0].descriptorCount = static_cast<uint32_t>(m_MaxFramesInFlight);
		descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorPoolSizes[1].descriptorCount = static_cast<uint32_t>(m_MaxFramesInFlight);

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.poolSizeCount = 2;
		descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
		descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(m_MaxFramesInFlight);

		LF_CORE_ASSERT(vkCreateDescriptorPool(LogicalDevice, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool) == VK_SUCCESS, "Failed to create descriptor pool!");

		// Create Descriptor sets
		std::vector<VkDescriptorSetLayout> layouts(m_MaxFramesInFlight, m_DescriptorSetLayout);
		VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
		descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocInfo.descriptorPool = m_DescriptorPool;
		descriptorSetAllocInfo.descriptorSetCount = static_cast<uint32_t>(m_MaxFramesInFlight);
		descriptorSetAllocInfo.pSetLayouts = layouts.data();

		DescriptorSets.resize(m_MaxFramesInFlight);
		LF_CORE_ASSERT(vkAllocateDescriptorSets(LogicalDevice, &descriptorSetAllocInfo, DescriptorSets.data()) == VK_SUCCESS, "Failed to allocate descriptor sets!");

		for (size_t i = 0; i < m_MaxFramesInFlight; i++) {

			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = m_UniformBuffers[i]->GetBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = Image1->GetImageView();
			imageInfo.sampler = m_Sampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = DescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;
			descriptorWrites[0].pImageInfo = nullptr;
			descriptorWrites[0].pTexelBufferView = nullptr;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = DescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = nullptr;
			descriptorWrites[1].pImageInfo = &imageInfo;
			descriptorWrites[1].pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(LogicalDevice, (uint32_t)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
		}

		// Create Commandbuffers
		CommandBuffers.resize(m_MaxFramesInFlight);

		VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.commandPool = m_CommandPool;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.commandBufferCount = (uint32_t)CommandBuffers.size();

		LF_CORE_ASSERT(vkAllocateCommandBuffers(LogicalDevice, &commandBufferAllocInfo, CommandBuffers.data()) == VK_SUCCESS, "Failed to allocate command buffers!");

		// Create sync objects
		ImageAvailableSemaphores.resize(m_MaxFramesInFlight);
		RenderFinishedSemaphores.resize(m_MaxFramesInFlight);
		InFlightFences.resize(m_MaxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < m_MaxFramesInFlight; i++) {

			LF_CORE_ASSERT(vkCreateSemaphore(LogicalDevice, &semaphoreCreateInfo, nullptr, &ImageAvailableSemaphores[i]) == VK_SUCCESS, "Failed to create imageAvailable semaphore!");
			LF_CORE_ASSERT(vkCreateSemaphore(LogicalDevice, &semaphoreCreateInfo, nullptr, &RenderFinishedSemaphores[i]) == VK_SUCCESS, "Failed to create renderFinished semaphore!");
			LF_CORE_ASSERT(vkCreateFence(LogicalDevice, &fenceCreateInfo, nullptr, &InFlightFences[i]) == VK_SUCCESS, "Failed to create inFlight fence!");
		}

	}
	
	void RenderContext::Shutdown() {

		Image1->Destroy();
		m_SwapChain->Destroy();
		VertexBuffer->Destroy();
		IndexBuffer->Destroy();

		for (auto buffer : m_UniformBuffers)
			buffer->Destroy();

		for (size_t i = 0; i < m_MaxFramesInFlight; i++) {

			vkDestroySemaphore(LogicalDevice, ImageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(LogicalDevice, RenderFinishedSemaphores[i], nullptr);
			vkDestroyFence(LogicalDevice, InFlightFences[i], nullptr);
		}

		vkDestroySampler(LogicalDevice, m_Sampler, nullptr);

		vkDestroyDescriptorPool(LogicalDevice, m_DescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(LogicalDevice, m_DescriptorSetLayout, nullptr);

		vkDestroyCommandPool(LogicalDevice, m_CommandPool, nullptr);
		vkDestroyPipeline(LogicalDevice, GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(LogicalDevice, PipelineLayout, nullptr);
		vkDestroyRenderPass(LogicalDevice, Renderpass, nullptr);

		m_DebugMessenger->Shutdown();

		vkDestroyDevice(LogicalDevice, nullptr);

		vkDestroySurfaceKHR(Instance, Surface, nullptr);

		vkDestroyInstance(Instance, nullptr);
	}

	void RenderContext::InitInstance() {

		VkApplicationInfo appInfo;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = NULL;
		appInfo.pApplicationName = m_Window->GetSpec().Title.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(LF_VERSION_MAJOR, LF_VERSION_MINOR, LF_VERSION_PATCH);
		appInfo.pEngineName = LF_ENGINE_NAME;
		appInfo.engineVersion = VK_MAKE_VERSION(LF_VERSION_MAJOR, LF_VERSION_MINOR, LF_VERSION_PATCH);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		auto extensions = Utils::GetRequiredVulkanExtensions();
		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
		instanceCreateInfo.enabledLayerCount = (uint32_t)(DebugMessenger::ValidationLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = DebugMessenger::ValidationLayers.data();
		instanceCreateInfo.pNext = nullptr;

		#ifdef LF_BE_OVERLYSPECIFIC
		// Add debug messenger to instance creation and cleanup
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
		Utils::PopulateDebugMessageCreateInfo(debugMessengerCreateInfo, m_DebugMessenger->GetMessageCallback());
		instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerCreateInfo;
		#endif

		LF_CORE_ASSERT(vkCreateInstance(&instanceCreateInfo, nullptr, &Instance) == VK_SUCCESS, "Failed to create instance!");
	}

	bool RenderContext::OnFramebufferResize(FramebufferResizeEvent& event) {

		m_SwapChain->Recreate();
		Application::GetInstance().OnUpdate();
		return true;
	}

	VkCommandBuffer RenderContext::BeginSingleTimeCommandBuffer() {

		VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.commandPool = m_CommandPool;
		commandBufferAllocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(LogicalDevice, &commandBufferAllocInfo, &commandBuffer);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		return commandBuffer;
	}

	void RenderContext::EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer) {

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(GraphicsQueueHandle, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(GraphicsQueueHandle);

		vkFreeCommandBuffers(LogicalDevice, m_CommandPool, 1, &commandBuffer);
	}

	void RenderContext::CopyBuffer(Ref<Buffer> srcBuffer, Ref<Buffer> dstBuffer) {

		VkCommandBuffer commandBuffer = BeginSingleTimeCommandBuffer();

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = dstBuffer->GetSize();
		vkCmdCopyBuffer(commandBuffer, srcBuffer->GetBuffer(), dstBuffer->GetBuffer(), 1, &copyRegion);

		EndSingleTimeCommandBuffer(commandBuffer);
	}

	void RenderContext::UpdateUniformBuffer(uint32_t currentImage) {

		float time = m_Timer.Elapsed();

		UniformBufferObject ubo = {};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), m_SwapChain->GetExtent().width / (float)m_SwapChain->GetExtent().height, 0.1f, 10.0f); // TODO: Fix error when minimizing window (SwapChainExtent.height becomes 0)
		ubo.proj[1][1] *= -1; // glm was designed for OpenGL, where the y-axis is flipped. This unflips it for Vulkan

		memcpy(m_UniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}

	void RenderContext::CreateImageSampler() {

		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties(PhysicalDevice, &properties);

		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.anisotropyEnable = VK_TRUE;
		samplerCreateInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK; // Unused: see addressModes
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;

		LF_CORE_ASSERT(vkCreateSampler(LogicalDevice, &samplerCreateInfo, nullptr, &m_Sampler) == VK_SUCCESS, "Failed to create image sampler!");

	}
}