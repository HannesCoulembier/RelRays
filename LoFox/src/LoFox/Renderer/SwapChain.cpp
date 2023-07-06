#include "lfpch.h"
#include "LoFox/Renderer/SwapChain.h"

#include "LoFox/Renderer/Renderer.h"
#include "LoFox/Utils/VulkanUtils.h"

#include "LoFox/Renderer/Image.h"

namespace LoFox {

	SwapChain::SwapChain(Ref<RenderContext> context, Ref<Window> window)
		: m_Context(context), m_Window(window) {

		CreateSwapChainAndImages();
		CreateImageViews();
		CreateDepthResources();
	}

	void SwapChain::Destroy() { CleanupSwapChain(); }

	void SwapChain::Recreate() {

		Renderer::WaitIdle();

		CleanupSwapChain();

		CreateSwapChainAndImages();
		CreateImageViews();
		CreateDepthResources();
		CreateFramebuffers();
	}

	void SwapChain::CreateSwapChainAndImages() {

		Utils::QueueFamilyIndices indices = Utils::IdentifyVulkanQueueFamilies(m_Context->PhysicalDevice, m_Context->Surface);

		Utils::SwapChainSupportDetails swapChainSupport = Utils::GetSwapChainSupportDetails(m_Context->PhysicalDevice, m_Context->Surface);

		VkSurfaceFormatKHR surfaceFormat = Utils::ChooseSwapSurfaceFormat(swapChainSupport.Formats);
		m_ImageFormat = surfaceFormat.format;
		VkPresentModeKHR presentMode = Utils::ChooseSwapPresentMode(swapChainSupport.PresentModes);
		m_Extent = Utils::ChooseSwapExtent(swapChainSupport.Capabilities, *m_Window);

		uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
		if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount) // when maxImageCount = 0, there is no limit
			imageCount = swapChainSupport.Capabilities.maxImageCount;

		uint32_t queueFamilyIndices[] = { indices.GraphicsFamilyIndex, indices.PresentFamilyIndex };

		VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
		swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCreateInfo.surface = m_Context->Surface;
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

		LF_CORE_ASSERT(vkCreateSwapchainKHR(m_Context->LogicalDevice, &swapChainCreateInfo, nullptr, &m_SwapChain) == VK_SUCCESS, "Failed to create swap chain!");

		// Retrieving the images in the swap chain
		vkGetSwapchainImagesKHR(m_Context->LogicalDevice, m_SwapChain, &imageCount, nullptr); // We only specified minImageCount, so swapchain might have created more. We reset imageCount to the actual number of images created.
		m_Images.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Context->LogicalDevice, m_SwapChain, &imageCount, m_Images.data());
	}

	void SwapChain::CreateImageViews() {

		m_ImageViews.resize(m_Images.size());
		for (uint32_t i = 0; i < m_Images.size(); i++) {

			VkImageViewCreateInfo imageViewCreateInfo{};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = m_Images[i];
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = m_ImageFormat;
			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			LF_CORE_ASSERT(vkCreateImageView(m_Context->LogicalDevice, &imageViewCreateInfo, nullptr, &m_ImageViews[i]) == VK_SUCCESS, "Failed to create image views!");
		}
	}

	void SwapChain::CreateDepthResources() {

		// Create depth resources
		VkFormat depthFormat = Utils::FindDepthFormat(m_Context->PhysicalDevice);
		m_DepthImage = CreateRef<Image>(m_Context, GetExtent().width, GetExtent().height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	}

	void SwapChain::CreateFramebuffers() {

		m_Framebuffers.resize(m_ImageViews.size());

		for (size_t i = 0; i < m_ImageViews.size(); i++) {

			std::array<VkImageView, 2> attachments = {
				m_ImageViews[i],
				m_DepthImage->GetImageView(),
			};

			VkFramebufferCreateInfo framebufferCreateInfo = {};
			framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferCreateInfo.renderPass = m_Context->GetGraphicsPipeline().RenderPass;
			framebufferCreateInfo.attachmentCount = (uint32_t)attachments.size();
			framebufferCreateInfo.pAttachments = attachments.data();
			framebufferCreateInfo.width = m_Extent.width;
			framebufferCreateInfo.height = m_Extent.height;
			framebufferCreateInfo.layers = 1;

			LF_CORE_ASSERT(vkCreateFramebuffer(m_Context->LogicalDevice, &framebufferCreateInfo, nullptr, &m_Framebuffers[i]) == VK_SUCCESS, "Failed to create framebuffer!");
		}
	}

	void SwapChain::CleanupSwapChain() {

		for (auto framebuffer : m_Framebuffers)
			vkDestroyFramebuffer(m_Context->LogicalDevice, framebuffer, nullptr);

		for (auto imageView : m_ImageViews)
			vkDestroyImageView(m_Context->LogicalDevice, imageView, nullptr);

		m_DepthImage->Destroy();

		vkDestroySwapchainKHR(m_Context->LogicalDevice, m_SwapChain, nullptr);
	}

	int SwapChain::AcquireNextImageIndex(uint32_t currentFrame) {

		uint32_t nextImageIndex;
		VkResult result = vkAcquireNextImageKHR(m_Context->LogicalDevice, m_SwapChain, UINT64_MAX, m_Context->ImageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &nextImageIndex);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			Recreate();
			return -1;
		}
		LF_CORE_ASSERT(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR, "Failed to acquire swapchain image!");
		return nextImageIndex;
	}

	void SwapChain::PresentImage(uint32_t imageIndex, VkSemaphore* waitSemaphores) {

		VkSwapchainKHR swapChains[] = { m_Context->GetSwapChain()->GetSwapChain() };
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = waitSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		VkResult result = vkQueuePresentKHR(m_Context->PresentQueueHandle, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
			m_Context->GetSwapChain()->Recreate();
		}
		else
			LF_CORE_ASSERT(result == VK_SUCCESS, "Failed to present swapchain image!");
	}
}