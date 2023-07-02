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

		inline VkDevice GetLogicalDevice() { return m_LogicalDevice; }

		void WaitIdle() { vkDeviceWaitIdle(m_LogicalDevice); }

		static Ref<RenderContext> Create();
	private:
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		void LinkReference(Ref<RenderContext> origin) { m_Context = origin; };
		static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanMessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		
		void InitInstance();
	private:
		Ref<Window> m_Window = nullptr;
		Ref<RenderContext> m_Context;

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

		VkCommandPool m_CommandPool = nullptr;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		VkQueue m_GraphicsQueueHandle = nullptr;
		VkQueue m_PresentQueueHandle = nullptr;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;

		int m_CurrentFrame = 0;
		const int m_MaxFramesInFlight = 2;
		static const std::vector<const char*> s_ValidationLayers;
	};
}