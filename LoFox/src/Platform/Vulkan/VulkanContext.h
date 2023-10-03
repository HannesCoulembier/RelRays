#pragma once

#include "LoFox/Renderer/GraphicsContext.h"

#include <vulkan/vulkan.h>

#include "Platform/Vulkan/DebugMessenger.h"

struct GLFWwindow;

namespace LoFox {

	class VulkanContext : public GraphicsContext {

	public:
		static void Init(GLFWwindow* windowHandle);
		static void Shutdown();

		static void BeginFrame(glm::vec3 clearColor);
		static void EndFrame();
		static void PresentFrame();
	public:
		inline static VkInstance Instance;
		inline static VkPhysicalDevice PhysicalDevice;
		inline static VkDevice LogicalDevice;
		inline static VkSurfaceKHR Surface;

		inline static VkQueue GraphicsQueueHandle;
		inline static VkQueue PresentQueueHandle;

		inline static VkCommandPool CommandPool;
		inline static VkCommandBuffer MainCommandBuffer;
	private:
		static void InitInstance();
		static void InitSurface();
		static void InitDevices();
		static void InitSwapchain();
		static void InitDefaultRenderPass();
		static void InitFramebuffers();
		static void InitSyncStructures();

		static void DestroySwapchain();
	private:
		inline static GLFWwindow* m_WindowHandle;
		inline static Ref<DebugMessenger> m_DebugMessenger;

		inline static VkSwapchainKHR m_Swapchain;
		inline static VkFormat m_SwapchainImageFormat;
		inline static VkExtent2D m_SwapchainExtent;
		inline static std::vector<VkImage> m_SwapchainImages;
		inline static std::vector<VkImageView> m_SwapchainImageViews;
		inline static uint32_t m_ThisFramesImageIndex = 0;

		inline static VkRenderPass m_RenderPass;
		inline static std::vector<VkFramebuffer> m_Framebuffers;

		inline static VkFence m_RenderFence;
		inline static VkSemaphore m_PresentSemaphore;
		inline static VkSemaphore m_RenderSemaphore;

	};

}