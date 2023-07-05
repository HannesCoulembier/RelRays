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
	class SwapChain;

	class RenderContext {

	public:
		RenderContext() = default;

		void Init(Ref<Window> window);
		void Shutdown();
		
		void OnResize(uint32_t width, uint32_t height) { OnFramebufferResize(FramebufferResizeEvent(width, height)); }

		inline Ref<SwapChain> GetSwapChain() { return m_SwapChain; }

		static Ref<RenderContext> Create();

		void UpdateUniformBuffer(uint32_t currentImage);
	public:
		VkInstance Instance = nullptr;
		VkPhysicalDevice PhysicalDevice = nullptr;
		VkDevice LogicalDevice = nullptr;

		VkRenderPass Renderpass;
		VkPipelineLayout PipelineLayout;
		VkPipeline GraphicsPipeline;

		std::vector<VkDescriptorSet> DescriptorSets;

		VkSurfaceKHR Surface = nullptr;

		Ref<Buffer> VertexBuffer;
		Ref<Buffer> IndexBuffer;

		std::vector<VkCommandBuffer> CommandBuffers;

		VkQueue GraphicsQueueHandle = nullptr;
		VkQueue PresentQueueHandle = nullptr;

		std::vector<VkSemaphore> ImageAvailableSemaphores;
		std::vector<VkSemaphore> RenderFinishedSemaphores;
		std::vector<VkFence> InFlightFences;
	private:
		void LinkReference(Ref<RenderContext> origin) { m_ThisContext = origin; };
		void OnEvent(Event& event);
		bool OnFramebufferResize(FramebufferResizeEvent& event);
		
		void InitInstance();

		void CopyBuffer(Ref<Buffer> srcBuffer, Ref<Buffer> dstBuffer);
	private:
		Ref<Window> m_Window = nullptr;
		Ref<RenderContext> m_ThisContext;
		Ref<DebugMessenger> m_DebugMessenger;

		Timer m_Timer;

		Ref<SwapChain> m_SwapChain;

		VkDescriptorSetLayout m_DescriptorSetLayout;

		std::vector<Ref<Buffer>> m_UniformBuffers;
		std::vector<void*> m_UniformBuffersMapped;

		VkDescriptorPool m_DescriptorPool;
		VkCommandPool m_CommandPool = nullptr;

		int m_CurrentFrame = 0;
		const int m_MaxFramesInFlight = 2;
	};
}