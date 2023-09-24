#include "lfpch.h"
#include "LoFox/Renderer/SwapChain.h"

#include "LoFox/Renderer/Renderer.h"
#include "Platform/Vulkan/Utils.h"

#include "LoFox/Renderer/Image.h"

#include "LoFox/Renderer/RenderContext.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace LoFox {

	SwapChain::SwapChain(Ref<Window> window)
		: m_Window(window) {

		CreateSwapChain();
		CreateImagesWithViews();
		CreateDepthResources();

		CreateSyncObjects();
		CreateCommandBuffers();
	}

	void SwapChain::Destroy() {

		CleanupSwapChain();

		for (size_t i = 0; i < Renderer::MaxFramesInFlight; i++) {

			vkDestroySemaphore(VulkanContext::LogicalDevice, m_ImageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(VulkanContext::LogicalDevice, m_RenderFinishedSemaphores[i], nullptr);
			vkDestroyFence(VulkanContext::LogicalDevice, m_InFlightFences[i], nullptr);
		}
	}

	void SwapChain::BeginFrame() {

		vkWaitForFences(VulkanContext::LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		m_ThisFramesImageIndex = AcquireNextImageIndex();
		if (m_ThisFramesImageIndex < 0) // SwapChain out of date?
			return;

		vkResetFences(VulkanContext::LogicalDevice, 1, &m_InFlightFences[m_CurrentFrame]);

		vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
	}

	void SwapChain::SubmitFrame() {

		if (m_ThisFramesImageIndex < 0)
			return;

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

		LF_CORE_ASSERT(vkQueueSubmit(VulkanContext::GraphicsQueueHandle, 1, &submitInfo, m_InFlightFences[m_CurrentFrame]) == VK_SUCCESS, "Failed to submit draw command buffer!");

		PresentImage(signalSemaphores);
		
		m_CurrentFrame = (m_CurrentFrame + 1) % Renderer::MaxFramesInFlight;
	}

	void SwapChain::Recreate() {

		Renderer::WaitIdle();

		CleanupSwapChain();

		CreateSwapChain();
		CreateImagesWithViews();
		CreateDepthResources();
	}

	void SwapChain::CreateSwapChain() {

		Utils::QueueFamilyIndices indices = Utils::IdentifyVulkanQueueFamilies(VulkanContext::PhysicalDevice, VulkanContext::Surface);

		Utils::SwapChainSupportDetails swapChainSupport = Utils::GetSwapChainSupportDetails(VulkanContext::PhysicalDevice, VulkanContext::Surface);

		VkSurfaceFormatKHR surfaceFormat = Utils::ChooseSwapSurfaceFormat(swapChainSupport.Formats);
		m_ImageFormat = surfaceFormat.format;
		VkPresentModeKHR presentMode = Utils::ChooseSwapPresentMode(swapChainSupport.PresentModes);
		int width, height;
		m_Window->GetFramebufferSize(&width, &height);
		m_Extent = Utils::ChooseSwapExtent(swapChainSupport.Capabilities, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
		if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount) // when maxImageCount = 0, there is no limit
			imageCount = swapChainSupport.Capabilities.maxImageCount;

		uint32_t queueFamilyIndices[] = { indices.GraphicsFamilyIndex, indices.PresentFamilyIndex };

		VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
		swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCreateInfo.surface = VulkanContext::Surface;
		swapChainCreateInfo.minImageCount = imageCount;
		swapChainCreateInfo.imageFormat = surfaceFormat.format;
		swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapChainCreateInfo.imageExtent = m_Extent;
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

		LF_CORE_ASSERT(vkCreateSwapchainKHR(VulkanContext::LogicalDevice, &swapChainCreateInfo, nullptr, &m_SwapChain) == VK_SUCCESS, "Failed to create swap chain!");
	}

	void SwapChain::CreateImagesWithViews() {

		// Retrieving the images in the swap chain
		uint32_t imageCount;
		vkGetSwapchainImagesKHR(VulkanContext::LogicalDevice, m_SwapChain, &imageCount, nullptr); // We only specified minImageCount, so swapchain might have created more. We reset imageCount to the actual number of images created.
		m_Images.resize(imageCount);
		std::vector<VkImage> images(imageCount);
		vkGetSwapchainImagesKHR(VulkanContext::LogicalDevice, m_SwapChain, &imageCount, images.data());

		// Creating the imageViews
		for (size_t i = 0; i < imageCount; i++) {

			m_Images[i].Image = images[i];
			m_Images[i].ImageView = Image::CreateImageView(m_Images[i].Image, m_ImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	void SwapChain::CreateDepthResources() {

		// Create depth resources
		VkFormat depthFormat = Utils::FindDepthFormat(VulkanContext::PhysicalDevice);
		m_DepthImage = CreateRef<Image>(GetExtent().width, GetExtent().height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

	}

	void SwapChain::CreateFramebuffers(VkRenderPass renderPass) {

		for (size_t i = 0; i < m_Images.size(); i++) {

			std::array<VkImageView, 2> attachments = {
				m_Images[i].ImageView,
				m_DepthImage->GetImageView(),
			};

			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = renderPass;
			framebufferCreateInfo.attachmentCount = (uint32_t)attachments.size();
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = m_Extent.width;
			framebufferCreateInfo.height = m_Extent.height;
			framebufferCreateInfo.layers = 1;

			LF_CORE_ASSERT(vkCreateFramebuffer(VulkanContext::LogicalDevice, &framebufferCreateInfo, nullptr, &m_Images[i].Framebuffer) == VK_SUCCESS, "Failed to create framebuffer!");
		}
	}

	void SwapChain::CleanupSwapChain() {

		for (auto frame : m_Images) {

			vkDestroyFramebuffer(VulkanContext::LogicalDevice, frame.Framebuffer, nullptr);
			vkDestroyImageView(VulkanContext::LogicalDevice, frame.ImageView, nullptr);
		}

		m_DepthImage->Destroy();

		vkDestroySwapchainKHR(VulkanContext::LogicalDevice, m_SwapChain, nullptr);
	}

	int SwapChain::AcquireNextImageIndex() {

		uint32_t nextImageIndex;
		VkResult result = vkAcquireNextImageKHR(VulkanContext::LogicalDevice, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphores[m_CurrentFrame], VK_NULL_HANDLE, &nextImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			Recreate();
			return -1;
		}
		LF_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swapchain image!");
		return nextImageIndex;
	}

	void SwapChain::PresentImage(VkSemaphore* waitSemaphores) {

		VkSwapchainKHR swapChains[] = { m_SwapChain };
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = waitSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_ThisFramesImageIndex;
		presentInfo.pResults = nullptr;

		VkResult result = vkQueuePresentKHR(VulkanContext::PresentQueueHandle, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			Recreate();
		}
		else
			LF_CORE_ASSERT(result == VK_SUCCESS, "Failed to present swapchain image!");
	}

	void SwapChain::CreateSyncObjects() {

		// Create sync objects
		m_ImageAvailableSemaphores.resize(Renderer::MaxFramesInFlight);
		m_RenderFinishedSemaphores.resize(Renderer::MaxFramesInFlight);
		m_InFlightFences.resize(Renderer::MaxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < Renderer::MaxFramesInFlight; i++) {

			LF_CORE_ASSERT(vkCreateSemaphore(VulkanContext::LogicalDevice, &semaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphores[i]) == VK_SUCCESS, "Failed to create imageAvailable semaphore!");
			LF_CORE_ASSERT(vkCreateSemaphore(VulkanContext::LogicalDevice, &semaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphores[i]) == VK_SUCCESS, "Failed to create renderFinished semaphore!");
			LF_CORE_ASSERT(vkCreateFence(VulkanContext::LogicalDevice, &fenceCreateInfo, nullptr, &m_InFlightFences[i]) == VK_SUCCESS, "Failed to create inFlight fence!");
		}
	}

	void SwapChain::CreateCommandBuffers() {

		m_CommandBuffers.resize(Renderer::MaxFramesInFlight);
		VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.commandPool = VulkanContext::CommandPool;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();

		LF_CORE_ASSERT(vkAllocateCommandBuffers(VulkanContext::LogicalDevice, &commandBufferAllocInfo, m_CommandBuffers.data()) == VK_SUCCESS, "Failed to allocate command buffers!");
	}
}