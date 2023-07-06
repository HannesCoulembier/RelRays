#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

#include "LoFox/Core/Window.h"
#include "LoFox/Events/Event.h"
#include "LoFox/Events/RenderEvent.h"

#include "LoFox/Renderer/Pipeline.h"

namespace LoFox {

	class DebugMessenger;

	class Buffer;
	class SwapChain;
	class Image;

	class RenderContext {

	public:
		RenderContext() = default;

		static void Init(Ref<Window> window);
		static void InitPipelines(VkDescriptorSetLayout descriptorSetLayout, VkVertexInputBindingDescription vertexBindingDescription, std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions);
		static void Shutdown();

		static inline Ref<SwapChain> GetSwapChain() { return m_SwapChain; }
		static inline GraphicsPipeline GetGraphicsPipeline() { return m_GraphicsPipeline; }

		static VkCommandBuffer BeginSingleTimeCommandBuffer();
		static void EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer);

		static void CopyBuffer(Ref<Buffer> srcBuffer, Ref<Buffer> dstBuffer);
	public:
		static VkInstance Instance;
		static VkPhysicalDevice PhysicalDevice;
		static VkDevice LogicalDevice;
		static VkSurfaceKHR Surface;

		static std::vector<VkCommandBuffer> CommandBuffers;

		static VkQueue GraphicsQueueHandle;
		static VkQueue PresentQueueHandle;

		static std::vector<VkSemaphore> ImageAvailableSemaphores;
		static std::vector<VkSemaphore> RenderFinishedSemaphores;
		static std::vector<VkFence> InFlightFences;
	private:
		static void InitInstance();
		static void InitDevices();
		static void CreateSyncObjects();
	private:
		static Ref<Window> m_Window;
		static Ref<DebugMessenger> m_DebugMessenger;

		static Ref<SwapChain> m_SwapChain;
		static GraphicsPipeline m_GraphicsPipeline;

		static VkCommandPool m_CommandPool;

		static int m_CurrentFrame;
		static const int m_MaxFramesInFlight = 2;
	};
}