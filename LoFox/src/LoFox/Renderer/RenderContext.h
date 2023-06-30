#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

#include "LoFox/Core/Window.h"

namespace LoFox {

	class RenderContext {

	public:
		RenderContext() = default;

		void Init(Ref<Window> window);
		void Shutdown();

		void OnRender();

		void WaitIdle() { vkDeviceWaitIdle(m_LogicalDevice); }

		static Ref<RenderContext> Create();
	private:
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanMessageCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);
	private:
		Ref<Window> m_Window = nullptr;
		std::vector<Window> tests;

		VkInstance m_Instance = nullptr;
		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkDevice m_LogicalDevice = nullptr;
		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
		VkSurfaceKHR m_Surface = nullptr;
		VkSwapchainKHR m_SwapChain = nullptr;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;

		VkRenderPass m_Renderpass;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;

		VkCommandPool m_CommandPool;
		VkCommandBuffer m_CommandBuffer;

		VkQueue m_GraphicsQueueHandle = nullptr;
		VkQueue m_PresentQueueHandle = nullptr;

		VkSemaphore m_ImageAvailableSemaphore;
		VkSemaphore m_RenderFinishedSemaphore;
		VkFence m_InFlightFence;

		static const std::vector<const char*> s_ValidationLayers;
	};
}