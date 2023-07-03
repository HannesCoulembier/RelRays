#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

#include "LoFox/Core/Window.h"

namespace LoFox {

	class DebugMessenger;

	class RenderContext {

	public:
		RenderContext() = default;

		void Init(Ref<Window> window);
		void Shutdown();

		void OnRender();

		inline VkInstance GetInstance() { return m_Instance; }
		inline VkDevice GetLogicalDevice() { return m_LogicalDevice; }

		void WaitIdle() { vkDeviceWaitIdle(m_LogicalDevice); }

		static Ref<RenderContext> Create();
	private:
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		void LinkReference(Ref<RenderContext> origin) { m_Context = origin; };
		
		void InitInstance();
		void RecreateSwapChain();

		void CleanupSwapChain();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateFramebuffers();
	private:
		Ref<Window> m_Window = nullptr;
		Ref<RenderContext> m_Context;
		Ref<DebugMessenger> m_DebugMessenger;

		VkInstance m_Instance = nullptr;
		VkPhysicalDevice m_PhysicalDevice = nullptr;
		VkDevice m_LogicalDevice = nullptr;
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

		bool m_FramebufferResized = false;

		int m_CurrentFrame = 0;
		const int m_MaxFramesInFlight = 2;
	};
}