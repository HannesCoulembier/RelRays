#pragma once
#include "LoFox/Renderer/GraphicsContext.h"

#include <vulkan/vulkan.h>

#include "Platform/Vulkan/VulkanDebugMessenger.h"
#include "LoFox/Renderer/VertexBuffer.h"
#include "LoFox/Renderer/IndexBuffer.h"

// Forward declaration
struct GLFWwindow;

namespace LoFox {

	// TODO: add double buffering

	class VulkanContext : public GraphicsContext {

	public:
		static void Init(GLFWwindow* windowHandle);
		static void Shutdown();

		static void BeginFrame(glm::vec3 clearColor);
		static void SetActivePipeline(Ref<GraphicsPipeline> pipeline);
		static void Draw(Ref<IndexBuffer> indexBuffer, Ref<VertexBuffer> vertexBuffer);
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
		inline static VkExtent2D SwapchainExtent;
	private:
		static void InitInstance();
		static void InitSurface();
		static void InitDevices();
		static void InitSwapchain();
		static void InitDefaultRenderPass();
		static void InitFramebuffers();
		static void InitSyncStructures();
		static void InitDescriptorPool();

		static void DestroySwapchain();
	private:
		inline static GLFWwindow* m_WindowHandle;
		inline static Ref<VulkanDebugMessenger> m_DebugMessenger;

		inline static VkSwapchainKHR m_Swapchain;
		inline static VkFormat m_SwapchainImageFormat;
		inline static std::vector<VkImage> m_SwapchainImages;
		inline static std::vector<VkImageView> m_SwapchainImageViews;
		inline static uint32_t m_ThisFramesImageIndex = 0;

		inline static std::vector<VkFramebuffer> m_Framebuffers;

		inline static VkFence m_RenderFence;
		inline static VkSemaphore m_PresentSemaphore;
		inline static VkSemaphore m_RenderSemaphore;

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
		inline static bool m_IsPipelineBound = false;
		inline static bool m_HasActiveRenderPass = false;
	};

}