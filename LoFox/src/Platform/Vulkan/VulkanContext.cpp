#include "lfpch.h"
#include "Platform/Vulkan/VulkanContext.h"

#include <GLFW/glfw3.h>

#include "LoFox/Renderer/Shader.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanVertexBuffer.h"
#include "Platform/Vulkan/VulkanPipeline.h"

#include "LoFox/Core/Application.h"
#include "Platform/Vulkan/Utils.h"

namespace LoFox {

	void VulkanContext::Init(GLFWwindow* windowHandle) {

		LF_CORE_ASSERT(windowHandle, "Window handle is null!");
		m_WindowHandle = windowHandle;

		LF_OVERSPECIFY("Creating Vulkan instance");
		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanExtensions();
		Utils::ListAvailableVulkanLayers();
		#endif

		InitInstance();
		m_DebugMessenger = DebugMessenger::Create();
		InitSurface();
		InitDevices();

		InitSwapchain();
		InitDefaultRenderPass();
		InitFramebuffers();

		InitSyncStructures();
	}

	void VulkanContext::Shutdown() {

		vkWaitForFences(LogicalDevice, 1, &m_RenderFence, VK_TRUE, UINT64_MAX); // Wait for the rendering to finish

		vkDestroyFence(LogicalDevice, m_RenderFence, nullptr);
		vkDestroySemaphore(LogicalDevice, m_RenderSemaphore, nullptr);
		vkDestroySemaphore(LogicalDevice, m_PresentSemaphore, nullptr);

		for (VkFramebuffer framebuffer : m_Framebuffers)
			vkDestroyFramebuffer(LogicalDevice, framebuffer, nullptr);
		vkDestroyRenderPass(LogicalDevice, RenderPass, nullptr);

		DestroySwapchain();

		vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);
		m_DebugMessenger->Shutdown();
		vkDestroyDevice(LogicalDevice, nullptr);
		vkDestroySurfaceKHR(Instance, Surface, nullptr);
		vkDestroyInstance(Instance, nullptr);
	}


	void VulkanContext::BeginFrame(glm::vec3 clearColor) {

		vkWaitForFences(LogicalDevice, 1, &m_RenderFence, VK_TRUE, UINT64_MAX);
		vkResetFences(LogicalDevice, 1, &m_RenderFence);

		LF_CORE_ASSERT(vkAcquireNextImageKHR(LogicalDevice, m_Swapchain, UINT64_MAX, m_PresentSemaphore, nullptr, &m_ThisFramesImageIndex) == VK_SUCCESS, "Failed to acquire new image index!"); // This will immediately return the next image index, but it will only signal m_PresentSemaphore when the image is actually available
		
		vkResetCommandBuffer(MainCommandBuffer, 0);
		VkCommandBufferBeginInfo cmdBeginInfo = {};
		cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // We will reset the MainCommandBuffer every frame
		cmdBeginInfo.pInheritanceInfo = nullptr;
		cmdBeginInfo.pNext = nullptr;
		cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		LF_CORE_ASSERT(vkBeginCommandBuffer(MainCommandBuffer, &cmdBeginInfo) == VK_SUCCESS, "Failed to begin recording command buffer!");

		VkClearValue clearValue = { { clearColor.r, clearColor.g, clearColor.b, 1.0f } };
		std::vector<VkClearValue> clearValues = { clearValue };

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.framebuffer = m_Framebuffers[m_ThisFramesImageIndex];
		renderPassInfo.pClearValues = clearValues.data();
		renderPassInfo.pNext = nullptr;
		renderPassInfo.renderArea.offset.x = 0;
		renderPassInfo.renderArea.offset.y = 0;
		renderPassInfo.renderArea.extent = SwapchainExtent;
		renderPassInfo.renderPass = RenderPass;
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

		vkCmdBeginRenderPass(MainCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	}

	void VulkanContext::SetActivePipeline(Ref<GraphicsPipeline> pipeline) {

		vkCmdBindPipeline(MainCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VulkanGraphicsPipelineData*>(pipeline->GetData())->Pipeline);
	}

	void VulkanContext::Draw(Ref<VertexBuffer> vertexBuffer) {

		VkBuffer buffer = static_cast<VulkanVertexBufferData*>(vertexBuffer->GetData())->Buffer->GetBuffer();
		std::vector<VkDeviceSize> offset = { 0 };
		vkCmdBindVertexBuffers(MainCommandBuffer, 0, 1, &buffer, offset.data());
		vkCmdDraw(MainCommandBuffer, 3, 1, 0, 0);
	}

	void VulkanContext::EndFrame() {

		vkCmdEndRenderPass(MainCommandBuffer);
		LF_CORE_ASSERT(vkEndCommandBuffer(MainCommandBuffer) == VK_SUCCESS, "Failed to record command buffer!");

		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submit = {};
		submit.commandBufferCount = 1;
		submit.pCommandBuffers = &MainCommandBuffer;
		submit.pNext = nullptr;
		submit.pSignalSemaphores = &m_RenderSemaphore; // Signal m_RenderSemaphore when rendering is finished
		submit.pWaitDstStageMask = &waitStage;
		submit.pWaitSemaphores = &m_PresentSemaphore; // Wait for the next image to be available
		submit.signalSemaphoreCount = 1;
		submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit.waitSemaphoreCount = 1;

		LF_CORE_ASSERT(vkQueueSubmit(GraphicsQueueHandle, 1, &submit, m_RenderFence) == VK_SUCCESS, "Failed to submit draw commands!");
	}

	void VulkanContext::PresentFrame() {

		VkPresentInfoKHR presentInfo = {};
		presentInfo.pImageIndices = &m_ThisFramesImageIndex;
		presentInfo.pNext = nullptr;
		// presentInfo.pResults = 
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.pWaitSemaphores = &m_RenderSemaphore; // Wait for the rendering to finish
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		presentInfo.waitSemaphoreCount = 1;

		LF_CORE_ASSERT(vkQueuePresentKHR(GraphicsQueueHandle, &presentInfo) == VK_SUCCESS, "Failed to present image!");
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
		instanceCreateInfo.enabledLayerCount = (uint32_t)(DebugMessenger::ValidationLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = DebugMessenger::ValidationLayers.data();
		instanceCreateInfo.pNext = nullptr;

		#ifdef LF_BE_OVERLYSPECIFIC
		// Add debug messenger to instance creation and cleanup
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = Utils::MakeDebugMessageCreateInfo(DebugMessenger::GetMessageCallback());
		instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerCreateInfo;
		#endif

		LF_CORE_ASSERT(vkCreateInstance(&instanceCreateInfo, nullptr, &Instance) == VK_SUCCESS, "Failed to create instance!");
	}

	void VulkanContext::InitSurface() {

		LF_CORE_ASSERT(glfwCreateWindowSurface(Instance, m_WindowHandle, nullptr, &Surface) == VK_SUCCESS, "Failed to create window surface!");
	}

	void VulkanContext::InitDevices() {

		// Pick physical device (= GPU to use)
		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanPhysicalDevices(VulkanContext::Instance);
		#endif

		PhysicalDevice = Utils::PickVulkanPhysicalDevice(VulkanContext::Instance, VulkanContext::Surface);

		// Create logical device
		// Figuring out what queues we want
		Utils::QueueFamilyIndices indices = Utils::IdentifyVulkanQueueFamilies(PhysicalDevice, VulkanContext::Surface);

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
		logicalDeviceCreateInfo.enabledLayerCount = (uint32_t)(DebugMessenger::ValidationLayers.size());
		logicalDeviceCreateInfo.ppEnabledLayerNames = DebugMessenger::ValidationLayers.data();
		logicalDeviceCreateInfo.pNext = nullptr;

		LF_CORE_ASSERT(vkCreateDevice(PhysicalDevice, &logicalDeviceCreateInfo, nullptr, &LogicalDevice) == VK_SUCCESS, "Failed to create logical device!");

		// Retrieve queue handles
		vkGetDeviceQueue(LogicalDevice, indices.GraphicsFamilyIndex, 0, &GraphicsQueueHandle);
		vkGetDeviceQueue(LogicalDevice, indices.PresentFamilyIndex, 0, &PresentQueueHandle);

		// Create Commandpool
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.pNext = nullptr;
		commandPoolCreateInfo.queueFamilyIndex = indices.GraphicsFamilyIndex;
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

		LF_CORE_ASSERT(vkCreateCommandPool(LogicalDevice, &commandPoolCreateInfo, nullptr, &CommandPool) == VK_SUCCESS, "Failed to create command pool!");

		// Create MainCommandBuffer
		VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.commandBufferCount = 1;
		commandBufferAllocInfo.commandPool = CommandPool;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.pNext = nullptr;
		LF_CORE_ASSERT(vkAllocateCommandBuffers(LogicalDevice, &commandBufferAllocInfo, &MainCommandBuffer) == VK_SUCCESS, "Failed to allocate main command buffer!");
	}

	void VulkanContext::InitSwapchain() {

		// Create Swapchain
		Utils::QueueFamilyIndices indices = Utils::IdentifyVulkanQueueFamilies(PhysicalDevice, Surface);

		Utils::SwapChainSupportDetails swapChainSupport = Utils::GetSwapchainSupportDetails(PhysicalDevice, Surface);

		VkSurfaceFormatKHR surfaceFormat = Utils::ChooseSwapchainSurfaceFormat(swapChainSupport.Formats);
		m_SwapchainImageFormat = surfaceFormat.format;
		VkPresentModeKHR presentMode = Utils::ChooseSwapchainPresentMode(swapChainSupport.PresentModes);

		int width, height;
		glfwGetFramebufferSize(m_WindowHandle, &width, &height);
		SwapchainExtent = Utils::ChooseSwapchainExtent(swapChainSupport.Capabilities, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
		if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount) // when maxImageCount = 0, there is no limit
			imageCount = swapChainSupport.Capabilities.maxImageCount;

		uint32_t queueFamilyIndices[] = { indices.GraphicsFamilyIndex, indices.PresentFamilyIndex };

		VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.surface = Surface;
		swapchainCreateInfo.minImageCount = imageCount;
		swapchainCreateInfo.imageFormat = surfaceFormat.format;
		swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent = SwapchainExtent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (indices.GraphicsFamilyIndex != indices.PresentFamilyIndex) {

			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreateInfo.queueFamilyIndexCount = 2; // Specifies there are 2 (graphics and present) families that will need to share the swapchain images
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		swapchainCreateInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.presentMode = presentMode;
		swapchainCreateInfo.clipped = VK_TRUE; // Pixels obscured by another window are ignored
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		LF_CORE_ASSERT(vkCreateSwapchainKHR(LogicalDevice, &swapchainCreateInfo, nullptr, &m_Swapchain) == VK_SUCCESS, "Failed to create swap chain!");
		
		// Create image(view)s
		vkGetSwapchainImagesKHR(LogicalDevice, m_Swapchain, &imageCount, nullptr); // We only specified minImageCount, so swapchain might have created more. We reset imageCount to the actual number of images created.
		
		m_SwapchainImages.resize(imageCount);
		m_SwapchainImageViews.resize(imageCount);
		vkGetSwapchainImagesKHR(LogicalDevice, m_Swapchain, &imageCount, m_SwapchainImages.data());

		// Creating the imageViews
		for (size_t i = 0; i < imageCount; i++) {

			m_SwapchainImageViews[i] = Utils::CreateImageViewFromImage(LogicalDevice, m_SwapchainImages[i], m_SwapchainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	void VulkanContext::InitDefaultRenderPass() {

		VkAttachmentDescription colorAttachment = {}; // The image we will render into
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // After the renderpass ends, the image will be displayed to the screen.
		// colorAttachment.flags = 
		colorAttachment.format = m_SwapchainImageFormat; // Use the format of the swapchain
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

	void VulkanContext::InitFramebuffers() {

		uint32_t swapchainImageCount = m_SwapchainImages.size();
		m_Framebuffers = std::vector<VkFramebuffer>(swapchainImageCount);

		for (int i = 0; i < swapchainImageCount; i++) {

			VkFramebufferCreateInfo framebufferInfo = {};
			framebufferInfo.attachmentCount = 1;
			// framebufferInfo.flags = 
			framebufferInfo.height = SwapchainExtent.height;
			framebufferInfo.layers = 1;
			framebufferInfo.pAttachments = &m_SwapchainImageViews[i];
			// framebufferInfo.pNext = 
			framebufferInfo.renderPass = RenderPass;
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.width = SwapchainExtent.width;

			LF_CORE_ASSERT(vkCreateFramebuffer(LogicalDevice, &framebufferInfo, nullptr, &m_Framebuffers[i]) == VK_SUCCESS, "Failed to create framebuffers!");
		}
	}

	void VulkanContext::InitSyncStructures() {

		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Fixes waiting on the fence in the first frame
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		LF_CORE_ASSERT(vkCreateFence(LogicalDevice, &fenceCreateInfo, nullptr, &m_RenderFence) == VK_SUCCESS, "Failed to create render fence!");

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.flags = 0;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		LF_CORE_ASSERT(vkCreateSemaphore(LogicalDevice, &semaphoreCreateInfo, nullptr, &m_PresentSemaphore) == VK_SUCCESS, "Failed to create present semaphore!");
		LF_CORE_ASSERT(vkCreateSemaphore(LogicalDevice, &semaphoreCreateInfo, nullptr, &m_RenderSemaphore) == VK_SUCCESS, "Failed to create render semaphore!");
	}

	void VulkanContext::DestroySwapchain() {

		for (auto imageView : m_SwapchainImageViews) {

			vkDestroyImageView(LogicalDevice, imageView, nullptr);
		}
		vkDestroySwapchainKHR(LogicalDevice, m_Swapchain, nullptr);
	}
}
