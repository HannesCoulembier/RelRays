#pragma once

#include <vulkan/vulkan.h>

#include "LoFox/Core/Window.h"

#include "LoFox/Renderer/Image.h"

namespace LoFox {

	struct SwapChainImage {

		VkImage Image;
		VkImageView ImageView;
		VkFramebuffer Framebuffer;
		VkCommandBuffer CommandBuffer;
	};

	class SwapChain {

	public:
		SwapChain(Ref<Window> window);

		void Destroy();

		void Recreate();

		void BeginFrame();
		void SubmitFrame();

		void CreateFramebuffers(VkRenderPass renderPass);

		inline VkSwapchainKHR GetSwapChain() { return m_SwapChain; }
		inline VkFramebuffer GetThisFramesFramebuffer() { return m_Images[m_ThisFramesImageIndex].Framebuffer; }
		inline VkCommandBuffer GetThisFramesCommandbuffer() { return m_Images[m_ThisFramesImageIndex].CommandBuffer; }
		inline int GetCurrentFrame() { return m_CurrentFrame; }

		inline Ref<Image> GetDepthImage() { return m_DepthImage; }
		inline VkFormat GetImageFormat() { return m_ImageFormat; }
		inline VkExtent2D GetExtent() { return m_Extent; }
	private:
		void CleanupSwapChain();
		void CreateSwapChain();
		void CreateImagesWithViews();
		void CreateDepthResources();

		int AcquireNextImageIndex();
		void PresentImage(VkSemaphore* waitSemaphores);

		void CreateSyncObjects();
		void CreateCommandBuffers();
	private:
		Ref<Window> m_Window;

		VkSwapchainKHR m_SwapChain;
		std::vector<SwapChainImage> m_Images; // size should correspond to the number of images available on the swapchain

		Ref<Image> m_DepthImage;

		VkFormat m_ImageFormat;
		VkExtent2D m_Extent;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;

		uint32_t m_ThisFramesImageIndex = 0;
		int m_CurrentFrame = 0;
	};
}