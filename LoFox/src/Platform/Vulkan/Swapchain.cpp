#include "lfpch.h"
#include "Platform/Vulkan/Swapchain.h"

#include "LoFox/Core/Window.h"

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/Utils.h"

namespace LoFox {

	Swapchain::Swapchain(Ref<Window> window) {

		m_Window = window;

		InitSwapchain();


		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Fixes waiting on the fence in the first frame
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		LF_CORE_ASSERT(vkCreateFence(VulkanContext::LogicalDevice, &fenceCreateInfo, nullptr, &m_InFlightFence) == VK_SUCCESS, "Failed to create render fence!");

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.flags = 0;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		LF_CORE_ASSERT(vkCreateSemaphore(VulkanContext::LogicalDevice, &semaphoreCreateInfo, nullptr, &m_ImageAvailableSemaphore) == VK_SUCCESS, "Failed to create image available semaphore!");
		LF_CORE_ASSERT(vkCreateSemaphore(VulkanContext::LogicalDevice, &semaphoreCreateInfo, nullptr, &m_RenderFinishedSemaphore) == VK_SUCCESS, "Failed to create render finished semaphore!");
	}

	void Swapchain::InitSwapchain() {

		// Create Swapchain
		Utils::QueueFamilyIndices indices = Utils::IdentifyVulkanQueueFamilies(VulkanContext::PhysicalDevice, VulkanContext::Surface);

		Utils::SwapChainSupportDetails swapChainSupport = Utils::GetSwapchainSupportDetails(VulkanContext::PhysicalDevice, VulkanContext::Surface);

		m_SurfaceFormat = Utils::ChooseSwapchainSurfaceFormat(swapChainSupport.Formats);
		m_ImageFormat = m_SurfaceFormat.format;
		m_PresentMode = Utils::ChooseSwapchainPresentMode(swapChainSupport.PresentModes);

		int width, height;
		m_Window->GetFramebufferSize(&width, &height);
		m_Extent = Utils::ChooseSwapchainExtent(swapChainSupport.Capabilities, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		m_MinImageCount = swapChainSupport.Capabilities.minImageCount + 1;
		if (swapChainSupport.Capabilities.maxImageCount > 0 && m_MinImageCount > swapChainSupport.Capabilities.maxImageCount) // when maxImageCount = 0, there is no limit
			m_MinImageCount = swapChainSupport.Capabilities.maxImageCount;

		uint32_t queueFamilyIndices[] = { indices.GraphicsFamilyIndex, indices.PresentFamilyIndex };

		VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.surface = VulkanContext::Surface;
		swapchainCreateInfo.minImageCount = m_MinImageCount;
		swapchainCreateInfo.imageFormat = m_ImageFormat;
		swapchainCreateInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent = m_Extent;
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
		swapchainCreateInfo.presentMode = m_PresentMode;
		swapchainCreateInfo.clipped = VK_TRUE; // Pixels obscured by another window are ignored
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		LF_CORE_ASSERT(vkCreateSwapchainKHR(VulkanContext::LogicalDevice, &swapchainCreateInfo, nullptr, &m_Swapchain) == VK_SUCCESS, "Failed to create swap chain!");

		// Create image(view)s
		vkGetSwapchainImagesKHR(VulkanContext::LogicalDevice, m_Swapchain, &m_ImageCount, nullptr); // We only specified minImageCount, so swapchain might have created more. We reset imageCount to the actual number of images created.

		m_Images.resize(m_ImageCount);
		vkGetSwapchainImagesKHR(VulkanContext::LogicalDevice, m_Swapchain, &m_ImageCount, m_Images.data());

		// Creating the imageViews
		m_ImageViews.resize(m_ImageCount);
		for (size_t i = 0; i < m_ImageCount; i++) {

			m_ImageViews[i] = Utils::CreateImageViewFromImage(VulkanContext::LogicalDevice, m_Images[i], m_ImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	void Swapchain::BeginFrame() {

		vkWaitForFences(VulkanContext::LogicalDevice, 1, &m_InFlightFence, VK_TRUE, UINT64_MAX);
		vkResetFences(VulkanContext::LogicalDevice, 1, &m_InFlightFence);

		VkResult result = vkAcquireNextImageKHR(VulkanContext::LogicalDevice, m_Swapchain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &m_ThisFramesImageIndex); // This will immediately return the next image index, but it will only signal m_PresentSemaphore when the image is actually available
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			Recreate();
			m_ThisFramesImageIndex -1;
		}
		LF_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swapchain image!");

		if (m_ThisFramesImageIndex < 0) // SwapChain out of date?
			return;

		// vkResetCommandBuffer(m_CommandBuffers[m_CurrentFrame], 0);
	}

	void Swapchain::EndFrame() {

		// submitting an empty commandbuffer to sync with the right semaphores
		VkPipelineStageFlags waitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo submit = {};
		submit.commandBufferCount = 0;
		submit.pCommandBuffers = nullptr;
		submit.pNext = nullptr;
		submit.pSignalSemaphores = &m_RenderFinishedSemaphore; // Signal m_RenderSemaphore when rendering is finished
		submit.pWaitDstStageMask = &waitStage;
		submit.pWaitSemaphores = &m_ImageAvailableSemaphore; // Wait for the next image to be available
		submit.signalSemaphoreCount = 1;
		submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit.waitSemaphoreCount = 1;

		LF_CORE_ASSERT(vkQueueSubmit(VulkanContext::GraphicsQueueHandle, 1, &submit, m_InFlightFence) == VK_SUCCESS, "Failed to submit draw commands!");
	}

	void Swapchain::PresentFrame() {

		VkPresentInfoKHR presentInfo = {};
		presentInfo.pImageIndices = &m_ThisFramesImageIndex;
		presentInfo.pNext = nullptr;
		// presentInfo.pResults = 
		presentInfo.pSwapchains = &m_Swapchain;
		presentInfo.pWaitSemaphores = &m_RenderFinishedSemaphore; // Wait for the rendering to finish
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.swapchainCount = 1;
		presentInfo.waitSemaphoreCount = 1;

		VkResult result = vkQueuePresentKHR(VulkanContext::PresentQueueHandle, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {

			Recreate();
		}
		else
			LF_CORE_ASSERT(result == VK_SUCCESS, "Failed to present swapchain image!");
	}

	void Swapchain::Recreate() {

		VulkanContext::WaitIdle();
		DestroySwapchain();
		InitSwapchain();

		for (auto f : m_DeathSubscribers)
			f();
	}

	void Swapchain::Destroy() {

		DestroySwapchain();

		vkDestroySemaphore(VulkanContext::LogicalDevice, m_ImageAvailableSemaphore, nullptr);
		vkDestroySemaphore(VulkanContext::LogicalDevice, m_RenderFinishedSemaphore, nullptr);
		vkDestroyFence(VulkanContext::LogicalDevice, m_InFlightFence, nullptr);
	}

	void Swapchain::DestroySwapchain() {

		for (auto imageView : m_ImageViews) {

			vkDestroyImageView(VulkanContext::LogicalDevice, imageView, nullptr);
		}

		vkDestroySwapchainKHR(VulkanContext::LogicalDevice, m_Swapchain, nullptr);
	}

	Ref<Swapchain> Swapchain::Create(Ref<Window> window) {

		return CreateRef<Swapchain>(window);
	}
}