#pragma once

#include <vulkan/vulkan.h>

#include "LoFox/Core/Window.h"

namespace LoFox {

	class Swapchain {

	public:
		void BeginFrame();
		void EndFrame();
		void PresentFrame();
		void Destroy();

		uint32_t GetThisFramesImageIndex() { return m_ThisFramesImageIndex; }
		VkFormat GetImageFormat() { return m_ImageFormat; }
		VkExtent2D GetExtent() { return m_Extent; }
		uint32_t GetMinImageCount() { return m_MinImageCount; }
		uint32_t GetImageCount() { return m_ImageCount; }
		VkImageView GetImageView(uint32_t index) { return m_ImageViews[index]; }

		void SubscribeToDeath(std::function<void(void)> func) { m_DeathSubscribers.push_back(func); }

		Swapchain(Ref<Window> window);
		static Ref<Swapchain> Create(Ref<Window> window);
	private:
		void InitSwapchain();
		void Recreate();
		void DestroySwapchain();
	private:
		Ref<Window> m_Window;
		std::vector<std::function<void()>> m_DeathSubscribers;

		VkSwapchainKHR m_Swapchain;
		std::vector<VkImageView> m_ImageViews;
		std::vector<VkImage> m_Images;
		
		VkExtent2D m_Extent;

		VkSurfaceFormatKHR m_SurfaceFormat;
		VkFormat m_ImageFormat;
		VkPresentModeKHR m_PresentMode;

		uint32_t m_MinImageCount;
		uint32_t m_ImageCount;

		uint32_t m_ThisFramesImageIndex = 0;

		VkFence m_InFlightFence;
		VkSemaphore m_ImageAvailableSemaphore;
		VkSemaphore m_RenderFinishedSemaphore;
	};
}