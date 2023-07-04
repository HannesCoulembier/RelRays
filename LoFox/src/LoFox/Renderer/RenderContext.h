#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

#include "LoFox/Core/Window.h"
#include "LoFox/Events/Event.h"
#include "LoFox/Events/RenderEvent.h"

#include "LoFox/Utils/Time.h"

namespace LoFox {

	class DebugMessenger;

	class Buffer;

	class RenderContext {

	public:
		RenderContext() = default;

		void Init(Ref<Window> window);
		void Shutdown();

		void OnRender();
		void OnResize(uint32_t width, uint32_t height) { OnFramebufferResize(FramebufferResizeEvent(width, height)); }

		static Ref<RenderContext> Create();

	public:
		VkInstance Instance = nullptr;
		VkPhysicalDevice PhysicalDevice = nullptr;
		VkDevice LogicalDevice = nullptr;

	private:
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		void UpdateUniformBuffer(uint32_t currentImage);

		void LinkReference(Ref<RenderContext> origin) { m_ThisContext = origin; };
		void OnEvent(Event& event);
		bool OnFramebufferResize(FramebufferResizeEvent& event);
		
		void InitInstance();
		void RecreateSwapChain();

		void CleanupSwapChain();
		void CreateSwapChain();
		void CreateImageViews();
		void CreateFramebuffers();

		void CopyBuffer(Ref<Buffer> srcBuffer, Ref<Buffer> dstBuffer);
	private:
		Ref<Window> m_Window = nullptr;
		Ref<RenderContext> m_ThisContext;
		Ref<DebugMessenger> m_DebugMessenger;

		Timer m_Timer;

		VkSurfaceKHR m_Surface = nullptr;
		VkSwapchainKHR m_SwapChain = nullptr;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;

		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;

		VkDescriptorPool m_DescriptorPool;
		std::vector<VkDescriptorSet> m_DescriptorSets;

		VkRenderPass m_Renderpass;
		VkDescriptorSetLayout m_DescriptorSetLayout;
		VkPipelineLayout m_PipelineLayout;
		VkPipeline m_GraphicsPipeline;

		Ref<Buffer> m_VertexBuffer;
		Ref<Buffer> m_IndexBuffer;

		std::vector<Ref<Buffer>> m_UniformBuffers;
		std::vector<void*> m_UniformBuffersMapped;

		VkCommandPool m_CommandPool = nullptr;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		VkQueue m_GraphicsQueueHandle = nullptr;
		VkQueue m_PresentQueueHandle = nullptr;

		std::vector<VkSemaphore> m_ImageAvailableSemaphores;
		std::vector<VkSemaphore> m_RenderFinishedSemaphores;
		std::vector<VkFence> m_InFlightFences;

		int m_CurrentFrame = 0;
		const int m_MaxFramesInFlight = 2;
	};
}