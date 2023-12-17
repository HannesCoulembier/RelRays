#include "lfpch.h"
#include "Platform/Vulkan/VulkanContext.h"

#include <GLFW/glfw3.h>

#include "LoFox/Renderer/Shader.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanVertexBuffer.h"
#include "Platform/Vulkan/VulkanIndexBuffer.h"
#include "Platform/Vulkan/VulkanFramebuffer.h"
#include "Platform/Vulkan/VulkanPipeline.h"

#include "LoFox/Core/Application.h"
#include "Platform/Vulkan/Utils.h"

namespace LoFox {

	void VulkanContext::Init(Ref<Window> window) {

		LF_CORE_ASSERT(window, "Window is null!");
		m_Window = window;

		LF_OVERSPECIFY("Creating Vulkan instance");
		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanExtensions();
		Utils::ListAvailableVulkanLayers();
		#endif

		InitInstance();
		m_DebugMessenger = VulkanDebugMessenger::Create();
		InitSurface();
		InitDevices();

		Swapchain = Swapchain::Create(m_Window);
		InitDefaultRenderPass();

		InitSyncStructures();
		InitDescriptorPool();
	}

	void VulkanContext::Shutdown() {

		// vkWaitForFences(LogicalDevice, 1, &m_RenderFence, VK_TRUE, UINT64_MAX); // Wait for the rendering to finish
		WaitIdle();

		vkDestroyDescriptorPool(LogicalDevice, MainDescriptorPool, nullptr);

		vkDestroyFence(LogicalDevice, m_ImmediateSubmitBackBone.SubmitFinishedFence, nullptr);

		vkDestroyRenderPass(LogicalDevice, RenderPass, nullptr);

		Swapchain->Destroy();

		vkDestroyCommandPool(LogicalDevice, m_ImmediateSubmitBackBone.CommandPool, nullptr);
		vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);
		m_DebugMessenger->Shutdown();
		vkDestroyDevice(LogicalDevice, nullptr);
		vkDestroySurfaceKHR(Instance, Surface, nullptr);
		vkDestroyInstance(Instance, nullptr);
	}

	void VulkanContext::BeginFrame() {

		Swapchain->BeginFrame();
	}

	void VulkanContext::BeginFramebuffer(Ref<Framebuffer> framebuffer, VkCommandBuffer commandBuffer, VkRenderPass renderPass, glm::vec3 clearColor) {

		m_ActiveFramebuffer = framebuffer;
		m_FrameData.ClearColor = clearColor;
		m_ActiveCommandBuffer = commandBuffer;
		m_ActiveRenderpass = renderPass;

		vkResetCommandBuffer(m_ActiveCommandBuffer, 0);
		VkCommandBufferBeginInfo cmdBeginInfo = {};
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // We will reset the commandBuffer every frame
		cmdBeginInfo.pInheritanceInfo = nullptr;
		cmdBeginInfo.pNext = nullptr;
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		LF_CORE_ASSERT(vkBeginCommandBuffer(m_ActiveCommandBuffer, &cmdBeginInfo) == VK_SUCCESS, "Failed to begin recording command buffer!");

		VkClearValue clearValue = { { m_FrameData.ClearColor.r, m_FrameData.ClearColor.g, m_FrameData.ClearColor.b, 1.0f } };
		std::vector<VkClearValue> clearValues = {
			clearValue,		// Image clearcolor
			{ 1.0f, 0 },	// Depth buffer clearcolor
		};

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.framebuffer = static_cast<VulkanFramebufferData*>(m_ActiveFramebuffer->GetData())->Framebuffer;
		renderPassInfo.pClearValues = clearValues.data();
		renderPassInfo.pNext = nullptr;
		renderPassInfo.renderArea.offset.x = 0;
		renderPassInfo.renderArea.offset.y = 0;
		renderPassInfo.renderArea.extent.width = m_ActiveFramebuffer->GetWidth();
		renderPassInfo.renderArea.extent.height = m_ActiveFramebuffer->GetHeight();
		renderPassInfo.renderPass = m_ActiveRenderpass;
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

		vkCmdBeginRenderPass(m_ActiveCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanContext::BeginFramebuffer(Ref<Framebuffer> framebuffer, glm::vec3 clearColor) {

		BeginFramebuffer(framebuffer, MainCommandBuffer, RenderPass, clearColor);
	}

	void VulkanContext::SetActivePipeline(Ref<GraphicsPipeline> pipeline) {

		m_ActivePipeline = pipeline;

		if (m_HasActiveRenderPass && m_IsPipelineBound) {

			VulkanGraphicsPipelineData* pipelineData = static_cast<VulkanGraphicsPipelineData*>(m_ActivePipeline->GetData());
			vkCmdBindPipeline(m_ActiveCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData->Pipeline);
			vkCmdBindDescriptorSets(m_ActiveCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData->Layout, 0, 1, &pipelineData->DescriptorSet, 0, nullptr);
		}
	}

	void VulkanContext::Draw(Ref<IndexBuffer> indexBuffer, Ref<VertexBuffer> vertexBuffer) {

		if (!m_IsPipelineBound) {

			VulkanGraphicsPipelineData* pipelineData = static_cast<VulkanGraphicsPipelineData*>(m_ActivePipeline->GetData());
			vkCmdBindPipeline(m_ActiveCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData->Pipeline);
			vkCmdBindDescriptorSets(m_ActiveCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineData->Layout, 0, 1, &pipelineData->DescriptorSet, 0, nullptr);
			m_IsPipelineBound = true;
		}

		VkViewport viewport = {};
		viewport.height = m_ActiveFramebuffer->GetHeight();
		viewport.maxDepth = 1.0f;
		viewport.minDepth = 0.0f;
		viewport.width = m_ActiveFramebuffer->GetWidth();
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		vkCmdSetViewport(m_ActiveCommandBuffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.extent.width = m_ActiveFramebuffer->GetWidth();
		scissor.extent.height = m_ActiveFramebuffer->GetHeight();
		scissor.offset.x = 0.0f;
		scissor.offset.y = 0.0f;
		vkCmdSetScissor(m_ActiveCommandBuffer, 0, 1, &scissor);

		VkBuffer vBuffer = static_cast<VulkanVertexBufferData*>(vertexBuffer->GetData())->Buffer->GetBuffer();
		VkBuffer iBuffer = static_cast<VulkanIndexBufferData*>(indexBuffer->GetData())->Buffer->GetBuffer();
		std::vector<VkDeviceSize> offset = { 0 };
		vkCmdBindVertexBuffers(m_ActiveCommandBuffer, 0, 1, &vBuffer, offset.data());
		vkCmdBindIndexBuffer(m_ActiveCommandBuffer, iBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(m_ActiveCommandBuffer, indexBuffer->GetNumberOfIndices(), 1, 0, 0, 0);
	}

	void VulkanContext::EndFramebuffer() {

		vkCmdEndRenderPass(m_ActiveCommandBuffer);
		m_HasActiveRenderPass = false;

		LF_CORE_ASSERT(vkEndCommandBuffer(m_ActiveCommandBuffer) == VK_SUCCESS, "Failed to record command buffer!");

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submit = {};
		submit.commandBufferCount = 1;
		submit.pCommandBuffers = &m_ActiveCommandBuffer;
		submit.pNext = nullptr;
		// submit.pSignalSemaphores = &m_RenderSemaphore; // Signal m_RenderSemaphore when rendering is finished
		// submit.pWaitDstStageMask = &waitStage;
		// submit.pWaitSemaphores = &m_PresentSemaphore; // Wait for the next image to be available
		// submit.signalSemaphoreCount = 1;
		submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		// submit.waitSemaphoreCount = 1;

		LF_CORE_ASSERT(vkQueueSubmit(GraphicsQueueHandle, 1, &submit, VK_NULL_HANDLE) == VK_SUCCESS, "Failed to submit draw commands!");
		m_IsPipelineBound = false;

		m_ActiveFramebuffer = nullptr;
	}

	void VulkanContext::EndFrame() {

		Swapchain->EndFrame();
	}

	void VulkanContext::PresentFrame() {

		Swapchain->PresentFrame();
	}

	VkCommandBuffer VulkanContext::BeginSingleTimeCommandBuffer() {

		VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.commandPool = CommandPool;
		commandBufferAllocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(LogicalDevice, &commandBufferAllocInfo, &commandBuffer);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		return commandBuffer;
	}

	void VulkanContext::EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer) {

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(GraphicsQueueHandle, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(GraphicsQueueHandle);

		vkFreeCommandBuffers(LogicalDevice, CommandPool, 1, &commandBuffer);
	}

	void VulkanContext::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function) {

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;
		commandBufferBeginInfo.pNext = nullptr;
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(m_ImmediateSubmitBackBone.CommandBuffer, &commandBufferBeginInfo);

		function(m_ImmediateSubmitBackBone.CommandBuffer);

		vkEndCommandBuffer(m_ImmediateSubmitBackBone.CommandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_ImmediateSubmitBackBone.CommandBuffer;
		submitInfo.pNext = nullptr;
		submitInfo.pSignalSemaphores = nullptr;
		submitInfo.pWaitDstStageMask = nullptr;
		submitInfo.pWaitSemaphores = nullptr;
		submitInfo.signalSemaphoreCount = 0;
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 0;

		vkQueueSubmit(GraphicsQueueHandle, 1, &submitInfo, m_ImmediateSubmitBackBone.SubmitFinishedFence);
		vkWaitForFences(LogicalDevice, 1, &m_ImmediateSubmitBackBone.SubmitFinishedFence, true, 999999);
		vkResetFences(LogicalDevice, 1, &m_ImmediateSubmitBackBone.SubmitFinishedFence);
		vkResetCommandPool(LogicalDevice, m_ImmediateSubmitBackBone.CommandPool, 0);
	}


	void VulkanContext::InitInstance() {

		VkApplicationInfo appInfo;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = NULL;
		appInfo.pApplicationName = Application::GetInstance().GetSpec().Name.c_str();
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
		instanceCreateInfo.enabledLayerCount = (uint32_t)(VulkanDebugMessenger::ValidationLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = VulkanDebugMessenger::ValidationLayers.data();
		instanceCreateInfo.pNext = nullptr;

		#ifdef LF_BE_OVERLYSPECIFIC
		// Add debug messenger to instance creation and cleanup
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = Utils::MakeDebugMessageCreateInfo(VulkanDebugMessenger::GetMessageCallback());
		instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerCreateInfo;
		#endif

		LF_CORE_ASSERT(vkCreateInstance(&instanceCreateInfo, nullptr, &Instance) == VK_SUCCESS, "Failed to create instance!");
	}

	void VulkanContext::InitSurface() {

		LF_CORE_ASSERT(glfwCreateWindowSurface(Instance, static_cast<GLFWwindow*>(m_Window->GetWindowHandle()), nullptr, &Surface) == VK_SUCCESS, "Failed to create window surface!");
	}

	void VulkanContext::InitDevices() {

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
		logicalDeviceFeatures.wideLines = VK_TRUE;

		VkDeviceCreateInfo logicalDeviceCreateInfo{};
		logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		logicalDeviceCreateInfo.queueCreateInfoCount = (uint32_t)(queueCreateInfos.size());
		logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		logicalDeviceCreateInfo.pEnabledFeatures = &logicalDeviceFeatures;
		logicalDeviceCreateInfo.enabledExtensionCount = (uint32_t)(Utils::requiredVulkanDeviceExtensions.size());
		logicalDeviceCreateInfo.ppEnabledExtensionNames = Utils::requiredVulkanDeviceExtensions.data();
		logicalDeviceCreateInfo.enabledLayerCount = (uint32_t)(VulkanDebugMessenger::ValidationLayers.size());
		logicalDeviceCreateInfo.ppEnabledLayerNames = VulkanDebugMessenger::ValidationLayers.data();
		logicalDeviceCreateInfo.pNext = nullptr;

		LF_CORE_ASSERT(vkCreateDevice(PhysicalDevice, &logicalDeviceCreateInfo, nullptr, &LogicalDevice) == VK_SUCCESS, "Failed to create logical device!");

		// Retrieve queue handles
		vkGetDeviceQueue(LogicalDevice, indices.GraphicsFamilyIndex, 0, &GraphicsQueueHandle);
		vkGetDeviceQueue(LogicalDevice, indices.PresentFamilyIndex, 0, &PresentQueueHandle);

		// Create Commandpools and CommandBuffers
		// Create Commandpool
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.pNext = nullptr;
		commandPoolCreateInfo.queueFamilyIndex = indices.GraphicsFamilyIndex;
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

		LF_CORE_ASSERT(vkCreateCommandPool(LogicalDevice, &commandPoolCreateInfo, nullptr, &CommandPool) == VK_SUCCESS, "Failed to create command pool!");
		
		// Create ImmediateSubmitBackBone Commandpool
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.pNext = nullptr;
		commandPoolCreateInfo.queueFamilyIndex = indices.GraphicsFamilyIndex;
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

		LF_CORE_ASSERT(vkCreateCommandPool(LogicalDevice, &commandPoolCreateInfo, nullptr, &m_ImmediateSubmitBackBone.CommandPool) == VK_SUCCESS, "Failed to create immediate submit command pool!");

		// Create MainCommandBuffer
		VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.commandBufferCount = 1;
		commandBufferAllocInfo.commandPool = CommandPool;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.pNext = nullptr;
		LF_CORE_ASSERT(vkAllocateCommandBuffers(LogicalDevice, &commandBufferAllocInfo, &MainCommandBuffer) == VK_SUCCESS, "Failed to allocate main command buffer!");
	
		// Create ImmediateSubmitBackBone CommandBuffer
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.commandBufferCount = 1;
		commandBufferAllocInfo.commandPool = m_ImmediateSubmitBackBone.CommandPool;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.pNext = nullptr;
		LF_CORE_ASSERT(vkAllocateCommandBuffers(LogicalDevice, &commandBufferAllocInfo, &m_ImmediateSubmitBackBone.CommandBuffer) == VK_SUCCESS, "Failed to allocate immediate submit command buffer!");

	}

	void VulkanContext::InitDefaultRenderPass() {

		VkAttachmentDescription colorAttachment = {}; // The image we will render into
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // After the renderpass ends, the image will be written to by ImGui
		// colorAttachment.flags = 
		colorAttachment.format = Swapchain->GetImageFormat(); // Use the format of the swapchain
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // We don't know nor care about the starting layout
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear the attachment when loading in
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // We don't use MSAA
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // We don't care about stencil
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Keep the attachment stored when the renderpass ends

		std::vector<VkAttachmentDescription> attachments = { colorAttachment };

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0; // Attachment is at index 0 in attachments array
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		std::vector<VkAttachmentReference> attachmentRefs = { colorAttachmentRef };

		VkSubpassDescription subpass = {};
		subpass.colorAttachmentCount = static_cast<uint32_t>(attachmentRefs.size());
		// subpass.flags = 
		// subpass.inputAttachmentCount = 
		subpass.pColorAttachments = attachmentRefs.data();
		// subpass.pDepthStencilAttachment = 
		// subpass.pInputAttachments = 
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		// subpass.pPreserveAttachments = 
		// subpass.preserveAttachmentCount = 
		// subpass.pResolveAttachments = 

		std::vector<VkSubpassDescription> subpasses = { subpass };

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		// renderPassInfo.dependencyCount = 
		// renderPassInfo.flags = 
		renderPassInfo.pAttachments = attachments.data();
		// renderPassInfo.pDependencies = 
		// renderPassInfo.pNext = 
		renderPassInfo.pSubpasses = subpasses.data();
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.subpassCount = subpasses.size();

		LF_CORE_ASSERT(vkCreateRenderPass(LogicalDevice, &renderPassInfo, nullptr, &RenderPass) == VK_SUCCESS, "Failed to create renderpass!");
	}

	void VulkanContext::InitSyncStructures() {
		
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.flags = 0;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		LF_CORE_ASSERT(vkCreateFence(LogicalDevice, &fenceCreateInfo, nullptr, &m_ImmediateSubmitBackBone.SubmitFinishedFence) == VK_SUCCESS, "Failed to create immediate submit fence!");
	}

	void VulkanContext::InitDescriptorPool() {

		std::vector<VkDescriptorPoolSize> sizes = {
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			10 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			10 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	10 },
		};

		VkDescriptorPoolCreateInfo poolCreateInfo = {};
		poolCreateInfo.flags = 0;
		poolCreateInfo.maxSets = 10;
		poolCreateInfo.pNext = nullptr;
		poolCreateInfo.poolSizeCount = (uint32_t)sizes.size();
		poolCreateInfo.pPoolSizes = sizes.data();
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

		LF_CORE_ASSERT(vkCreateDescriptorPool(LogicalDevice, &poolCreateInfo, nullptr, &MainDescriptorPool) == VK_SUCCESS, "Failed to create main descriptor pool!");
	}
}
