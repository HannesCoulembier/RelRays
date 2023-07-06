#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

#include "LoFox/Core/Window.h"
#include "LoFox/Events/Event.h"
#include "LoFox/Events/RenderEvent.h"

#include "LoFox/Renderer/Pipeline.h"

#include "LoFox/Utils/Time.h"

namespace LoFox {

	class DebugMessenger;

	class Buffer;
	class SwapChain;
	class Image;

	class RenderContext {

	public:
		RenderContext() = default;

		void Init(Ref<Window> window);
		void Shutdown();
		
		void OnResize(uint32_t width, uint32_t height) { OnFramebufferResize(FramebufferResizeEvent(width, height)); }

		inline Ref<SwapChain> GetSwapChain() { return m_SwapChain; }
		inline GraphicsPipeline GetGraphicsPipeline() { return m_GraphicsPipeline; }

		static Ref<RenderContext> Create();

		void UpdateUniformBuffer(uint32_t currentImage);

		VkCommandBuffer BeginSingleTimeCommandBuffer();
		void EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer);

		void CopyBuffer(Ref<Buffer> srcBuffer, Ref<Buffer> dstBuffer);
	public:
		VkInstance Instance = nullptr;
		VkPhysicalDevice PhysicalDevice = nullptr;
		VkDevice LogicalDevice = nullptr;

		std::vector<VkDescriptorSet> DescriptorSets;

		VkSurfaceKHR Surface = nullptr;

		Ref<Image> Texture1;

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
		bool OnFramebufferResize(FramebufferResizeEvent& event);

		void InitInstance();
		void CreateImageSampler();
	private:
		Ref<Window> m_Window = nullptr;
		Ref<RenderContext> m_ThisContext;
		Ref<DebugMessenger> m_DebugMessenger;

		Timer m_Timer;

		Ref<SwapChain> m_SwapChain;

		VkSampler m_Sampler;

		VkDescriptorSetLayout m_DescriptorSetLayout;

		GraphicsPipeline m_GraphicsPipeline;

		std::vector<Ref<Buffer>> m_UniformBuffers;
		std::vector<void*> m_UniformBuffersMapped;

		VkDescriptorPool m_DescriptorPool;
		VkCommandPool m_CommandPool = nullptr;

		int m_CurrentFrame = 0;
		const int m_MaxFramesInFlight = 2;
	};
}