#pragma once
#include "LoFox/Renderer/GraphicsContext.h"

#include <vulkan/vulkan.h>

#include "Platform/Vulkan/VulkanDebugMessenger.h"
#include "Platform/Vulkan/Swapchain.h"
#include "LoFox/Renderer/VertexBuffer.h"
#include "LoFox/Renderer/IndexBuffer.h"

namespace LoFox {

	// TODO: add double buffering

	class VulkanContext : public GraphicsContext {

	public:
		static void Init(Ref<Window> window);
		static void Shutdown();

		static void BeginFrame();
		static void BeginFramebuffer(Ref<Framebuffer> framebuffer, glm::vec3 clearColor);
		static void BeginFramebuffer(Ref<Framebuffer> framebuffer, VkCommandBuffer commandBuffer, VkRenderPass renderPass, glm::vec3 clearColor);
		static void SetActivePipeline(Ref<GraphicsPipeline> pipeline);
		static void Draw(Ref<IndexBuffer> indexBuffer, Ref<VertexBuffer> vertexBuffer);
		static void EndFramebuffer();
		static void EndFrame();
		static void PresentFrame();

		static void WaitIdle() { vkDeviceWaitIdle(LogicalDevice); }

		static VkCommandBuffer BeginSingleTimeCommandBuffer();
		static void EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer);
		static void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);
	public:
		inline static VkInstance Instance;
		inline static VkPhysicalDevice PhysicalDevice;
		inline static VkDevice LogicalDevice;
		inline static VkSurfaceKHR Surface;

		inline static VkQueue GraphicsQueueHandle;
		inline static VkQueue PresentQueueHandle;

		inline static VkCommandPool CommandPool;
		inline static VkCommandBuffer MainCommandBuffer;

		inline static VkDescriptorPool MainDescriptorPool;

		inline static VkRenderPass RenderPass;
		inline static Ref<Swapchain> Swapchain;
	private:
		static void InitInstance();
		static void InitSurface();
		static void InitDevices();
		static void InitDefaultRenderPass();
		static void InitSyncStructures();
		static void InitDescriptorPool();
	private:
		inline static Ref<Window> m_Window;
		inline static Ref<VulkanDebugMessenger> m_DebugMessenger;

		struct ImmediateSubmitBackBone {

			VkFence SubmitFinishedFence;
			VkCommandBuffer CommandBuffer;
			VkCommandPool CommandPool;
		};
		inline static ImmediateSubmitBackBone m_ImmediateSubmitBackBone;
		
		struct FrameData {
			glm::vec3 ClearColor;
		};

		inline static FrameData m_FrameData;

		inline static Ref<GraphicsPipeline> m_ActivePipeline = nullptr;
		inline static Ref<Framebuffer> m_ActiveFramebuffer = nullptr;
		inline static VkCommandBuffer m_ActiveCommandBuffer = {};
		inline static VkRenderPass m_ActiveRenderpass = {};
		inline static bool m_IsPipelineBound = false;
		inline static bool m_HasActiveRenderPass = false;
	};

}