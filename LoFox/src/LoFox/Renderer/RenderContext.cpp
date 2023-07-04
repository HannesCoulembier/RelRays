#include "lfpch.h"
#include "LoFox/Renderer/RenderContext.h"

#include <glm/glm.hpp>

#include "LoFox/Renderer/Shader.h"
#include "LoFox/Renderer/Buffer.h"
#include "LoFox/Renderer/DebugMessenger.h"

#include "LoFox/Core/Settings.h"
#include "LoFox/Utils/VulkanUtils.h"
#include "LoFox/Utils/Utils.h"

#include "LoFox/Events/RenderEvent.h"


struct Vertex {
	glm::vec2 Position;
	glm::vec3 Color;

	static VkVertexInputBindingDescription GetBindingDescription() {

		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> GetAttributeDescriptions() {

		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, Color);

		return attributeDescriptions;
	}
};

const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
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

		m_Window = window;
		m_Window->SetRenderEventCallback(LF_BIND_EVENT_FN(RenderContext::OnEvent));

		LF_OVERSPECIFY("Creating Vulkan instance");
		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanExtensions();
		Utils::ListAvailableVulkanLayers();
		#endif

		m_DebugMessenger = DebugMessenger::Create(m_Context);

		InitInstance();

		m_DebugMessenger->Init();

		// Setting up m_Surface
		m_Window->CreateVulkanSurface(m_Instance, &m_Surface);

		// Pick physical device (= GPU to use)
		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanPhysicalDevices(m_Instance);
		#endif

		m_PhysicalDevice = Utils::PickVulkanPhysicalDevice(m_Instance, m_Surface);

		// Create logical device
		// Figuring out what queues we want
		Utils::QueueFamilyIndices indices = Utils::IdentifyVulkanQueueFamilies(m_PhysicalDevice, m_Surface);

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

		VkDeviceCreateInfo logicalDeviceCreateInfo{};
		logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		logicalDeviceCreateInfo.pEnabledFeatures = &logicalDeviceFeatures;
		logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(Utils::requiredVulkanDeviceExtensions.size());
		logicalDeviceCreateInfo.ppEnabledExtensionNames = Utils::requiredVulkanDeviceExtensions.data();
		logicalDeviceCreateInfo.enabledLayerCount = (uint32_t)(m_DebugMessenger->GetValidationLayers().size());
		logicalDeviceCreateInfo.ppEnabledLayerNames = m_DebugMessenger->GetValidationLayers().data();
		logicalDeviceCreateInfo.pNext = nullptr;

		LF_CORE_ASSERT(vkCreateDevice(m_PhysicalDevice, &logicalDeviceCreateInfo, nullptr, &m_LogicalDevice) == VK_SUCCESS, "Failed to create logical device!");

		// Retrieve queue handles
		vkGetDeviceQueue(m_LogicalDevice, indices.GraphicsFamilyIndex, 0, &m_GraphicsQueueHandle);
		vkGetDeviceQueue(m_LogicalDevice, indices.PresentFamilyIndex, 0, &m_PresentQueueHandle);

		// Create SwapChain
		CreateSwapChain();

		// Create the image views
		CreateImageViews();

		// Create Render pass
		VkAttachmentDescription colorAttachmentDescription = {};
		colorAttachmentDescription.format = m_SwapChainImageFormat;
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

		LF_CORE_ASSERT(vkCreateRenderPass(m_LogicalDevice, &renderPassCreateInfo, nullptr, &m_Renderpass) == VK_SUCCESS, "Failed to create render pass!");

		// Create Graphics pipeline
		Shader vertexShader(m_Context, "Assets/Shaders/VertexShader.vert", ShaderType::Vertex);
		Shader fragmentShader(m_Context, "Assets/Shaders/FragmentShader.frag", ShaderType::Fragment);

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

		VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
		rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerCreateInfo.depthClampEnable = VK_FALSE;
		rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerCreateInfo.lineWidth = 1.0f;
		rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
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
		pipelineLayoutCreateInfo.setLayoutCount = 0;
		pipelineLayoutCreateInfo.pSetLayouts = nullptr;
		pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
		pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

		LF_CORE_ASSERT(vkCreatePipelineLayout(m_LogicalDevice, &pipelineLayoutCreateInfo, nullptr, &m_PipelineLayout) == VK_SUCCESS, "failed to create pipeline layout!");

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
		pipelineCreateInfo.layout = m_PipelineLayout;
		pipelineCreateInfo.renderPass = m_Renderpass;
		pipelineCreateInfo.subpass = 0;
		pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineCreateInfo.basePipelineIndex = -1;

		LF_CORE_ASSERT(vkCreateGraphicsPipelines(m_LogicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &m_GraphicsPipeline) == VK_SUCCESS, "Failed to create graphics pipeline!");

		// Create Framebuffers
		CreateFramebuffers();

		// Create Commandpool
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = indices.GraphicsFamilyIndex;

		LF_CORE_ASSERT(vkCreateCommandPool(m_LogicalDevice, &commandPoolCreateInfo, nullptr, &m_CommandPool) == VK_SUCCESS, "Failed to create command pool!");

		// Create Vertex buffer
		{ // This is a scope to ensure the destructor of vertexStagingBuffer is called
			uint32_t vertexBufferSize = sizeof(vertices[0]) * vertices.size();
			Ref<Buffer> vertexStagingBuffer = CreateRef<Buffer>(m_Context, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			m_VertexBuffer = CreateRef<Buffer>(m_Context, vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			vertexStagingBuffer->SetData(vertices.data());

			CopyBuffer(vertexStagingBuffer, m_VertexBuffer);
		}
		// Create Index buffer
		{
			uint32_t indexBufferSize = sizeof(vertexIndices[0]) * vertexIndices.size();
			Ref<Buffer> indexStagingBuffer = CreateRef<Buffer>(m_Context, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			m_IndexBuffer = CreateRef<Buffer>(m_Context, indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			indexStagingBuffer->SetData(vertexIndices.data());

			CopyBuffer(indexStagingBuffer, m_IndexBuffer);
		}

		// Create Commandbuffers
		m_CommandBuffers.resize(m_MaxFramesInFlight);

		VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.commandPool = m_CommandPool;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

		LF_CORE_ASSERT(vkAllocateCommandBuffers(m_LogicalDevice, &commandBufferAllocInfo, m_CommandBuffers.data()) == VK_SUCCESS, "Failed to allocate command buffers!");

		// Create sync objects
		m_ImageAvailableSemaphores.resize(m_MaxFramesInFlight);
		m_RenderFinishedSemaphores.resize(m_MaxFramesInFlight);
		m_InFlightFences.resize(m_MaxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < m_MaxFramesInFlight; i++) {

			LF_CORE_ASSERT(vkCreateSemaphore(m_LogicalDevice, &semaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphores[i]) == VK_SUCCESS, "Failed to create imageAvailable semaphore!");
			LF_CORE_ASSERT(vkCreateSemaphore(m_LogicalDevice, &semaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphores[i]) == VK_SUCCESS, "Failed to create renderFinished semaphore!");
			LF_CORE_ASSERT(vkCreateFence(m_LogicalDevice, &fenceCreateInfo, nullptr, &m_InFlightFences[i]) == VK_SUCCESS, "Failed to create inFlight fence!");
		}

	}
	
	void RenderContext::Shutdown() {

		for (size_t i = 0; i < m_MaxFramesInFlight; i++) {

			vkDestroySemaphore(m_LogicalDevice, m_ImageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(m_LogicalDevice, m_RenderFinishedSemaphores[i], nullptr);
			vkDestroyFence(m_LogicalDevice, m_InFlightFences[i], nullptr);
		}

		CleanupSwapChain();

		vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);
		vkDestroyPipeline(m_LogicalDevice, m_GraphicsPipeline, nullptr);
		vkDestroyPipelineLayout(m_LogicalDevice, m_PipelineLayout, nullptr);
		vkDestroyRenderPass(m_LogicalDevice, m_Renderpass, nullptr);

		m_DebugMessenger->Shutdown();

		vkDestroyDevice(m_LogicalDevice, nullptr);

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);

		vkDestroyInstance(m_Instance, nullptr);
	}

	void RenderContext::OnRender() {

		vkWaitForFences(m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(m_LogicalDevice, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapChain();
			return;
		}
		LF_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swapchain image!");

		vkResetFences(m_LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame]);

		vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
		RecordCommandBuffer(m_CommandBuffers[m_CurrentFrame], imageIndex);

		VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphores[m_CurrentFrame] };
		VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphores[m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffers[m_CurrentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		LF_CORE_ASSERT(vkQueueSubmit(m_GraphicsQueueHandle, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) == VK_SUCCESS, "Failed to submit draw command buffer!");

		VkSwapchainKHR swapChains[] = { m_SwapChain };
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(m_PresentQueueHandle, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			RecreateSwapChain();
		}
		else
			LF_CORE_ASSERT(result == VK_SUCCESS, "Failed to present swapchain image!");

		m_CurrentFrame = (m_CurrentFrame + 1) % m_MaxFramesInFlight;
	}

	void RenderContext::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		LF_CORE_ASSERT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS, "Failed to begin recording command buffer!");

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = m_Renderpass;
		renderPassBeginInfo.framebuffer = m_SwapChainFramebuffers[imageIndex];
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = m_SwapChainExtent;
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline);

		VkBuffer vertexBuffers[] = { m_VertexBuffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_SwapChainExtent.width);
		viewport.height = static_cast<float>(m_SwapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = m_SwapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdDrawIndexed(commandBuffer, (uint32_t)vertexIndices.size(), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		LF_CORE_ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS, "Failed to record command buffer!");
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
		instanceCreateInfo.enabledLayerCount = (uint32_t)(m_DebugMessenger->GetValidationLayers().size());
		instanceCreateInfo.ppEnabledLayerNames = m_DebugMessenger->GetValidationLayers().data();
		instanceCreateInfo.pNext = nullptr;

		#ifdef LF_BE_OVERLYSPECIFIC
		// Add debug messenger to instance creation and cleanup
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
		Utils::PopulateDebugMessageCreateInfo(debugMessengerCreateInfo, m_DebugMessenger->GetMessageCallback());
		instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerCreateInfo;
		#endif

		LF_CORE_ASSERT(vkCreateInstance(&instanceCreateInfo, nullptr, &m_Instance) == VK_SUCCESS, "Failed to create instance!");
	}

	void RenderContext::CleanupSwapChain() {

		for (auto framebuffer : m_SwapChainFramebuffers)
			vkDestroyFramebuffer(m_LogicalDevice, framebuffer, nullptr);

		for (auto imageView : m_SwapChainImageViews)
			vkDestroyImageView(m_LogicalDevice, imageView, nullptr);

		vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, nullptr);
	}

	void RenderContext::RecreateSwapChain() {

		WaitIdle();

		CleanupSwapChain();

		CreateSwapChain();
		CreateImageViews();
		CreateFramebuffers();
	}

	void RenderContext::CreateSwapChain() {

		Utils::QueueFamilyIndices indices = Utils::IdentifyVulkanQueueFamilies(m_PhysicalDevice, m_Surface);

		Utils::SwapChainSupportDetails swapChainSupport = Utils::GetSwapChainSupportDetails(m_PhysicalDevice, m_Surface);

		VkSurfaceFormatKHR surfaceFormat = Utils::ChooseSwapSurfaceFormat(swapChainSupport.Formats);
		m_SwapChainImageFormat = surfaceFormat.format;
		VkPresentModeKHR presentMode = Utils::ChooseSwapPresentMode(swapChainSupport.PresentModes);
		m_SwapChainExtent = Utils::ChooseSwapExtent(swapChainSupport.Capabilities, *m_Window);

		uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
		if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount) // when maxImageCount = 0, there is no limit
			imageCount = swapChainSupport.Capabilities.maxImageCount;

		uint32_t queueFamilyIndices[] = { indices.GraphicsFamilyIndex, indices.PresentFamilyIndex };

		VkSwapchainCreateInfoKHR swapChainCreateInfo{};
		swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCreateInfo.surface = m_Surface;
		swapChainCreateInfo.minImageCount = imageCount;
		swapChainCreateInfo.imageFormat = surfaceFormat.format;
		swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapChainCreateInfo.imageExtent = m_SwapChainExtent;
		swapChainCreateInfo.imageArrayLayers = 1;
		swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (indices.GraphicsFamilyIndex != indices.PresentFamilyIndex) {

			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapChainCreateInfo.queueFamilyIndexCount = 2; // Specifies there are 2 (graphics and present) families that will need to share the swapchain images
			swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
		else {
			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		swapChainCreateInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
		swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainCreateInfo.presentMode = presentMode;
		swapChainCreateInfo.clipped = VK_TRUE; // Pixels obscured by another window are ignored
		swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		LF_CORE_ASSERT(vkCreateSwapchainKHR(m_LogicalDevice, &swapChainCreateInfo, nullptr, &m_SwapChain) == VK_SUCCESS, "Failed to create swap chain!");

		// Retrieving the images in the swap chain
		vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imageCount, nullptr); // We only specified minImageCount, so swapchain might have created more. We reset imageCount to the actual number of images created.
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imageCount, m_SwapChainImages.data());
	}

	void RenderContext::CreateImageViews() {

		m_SwapChainImageViews.resize(m_SwapChainImages.size());
		for (uint32_t i = 0; i < m_SwapChainImages.size(); i++) {

			VkImageViewCreateInfo imageViewCreateInfo{};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = m_SwapChainImages[i];
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = m_SwapChainImageFormat;
			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			LF_CORE_ASSERT(vkCreateImageView(m_LogicalDevice, &imageViewCreateInfo, nullptr, &m_SwapChainImageViews[i]) == VK_SUCCESS, "Failed to create image views!");
		}
	}

	void RenderContext::CreateFramebuffers() {

		m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

		for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {

			VkImageView attachments[] = {
				m_SwapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = m_Renderpass;
			framebufferCreateInfo.attachmentCount = 1;
			framebufferCreateInfo.pAttachments = attachments;
			framebufferCreateInfo.width = m_SwapChainExtent.width;
			framebufferCreateInfo.height = m_SwapChainExtent.height;
			framebufferCreateInfo.layers = 1;

			LF_CORE_ASSERT(vkCreateFramebuffer(m_LogicalDevice, &framebufferCreateInfo, nullptr, &m_SwapChainFramebuffers[i]) == VK_SUCCESS, "Failed to create framebuffer!");
		}
	}

	void RenderContext::OnEvent(Event& event) {

		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<FramebufferResizeEvent>(LF_BIND_EVENT_FN(RenderContext::OnFramebufferResize));
	}

	bool RenderContext::OnFramebufferResize(FramebufferResizeEvent& event) {

		RecreateSwapChain();
		if (!m_Window->IsMinimized())
			OnRender();
		return true;
	}

	void RenderContext::CopyBuffer(Ref<Buffer> srcBuffer, Ref<Buffer> dstBuffer) {

		VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.commandPool = m_CommandPool;
		commandBufferAllocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_LogicalDevice, &commandBufferAllocInfo, &commandBuffer);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = dstBuffer->GetSize();
		vkCmdCopyBuffer(commandBuffer, srcBuffer->GetBuffer(), dstBuffer->GetBuffer(), 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_GraphicsQueueHandle, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_GraphicsQueueHandle);

		vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &commandBuffer);
	}
}