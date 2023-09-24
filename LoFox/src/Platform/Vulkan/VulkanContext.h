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
	private:
		inline static GLFWwindow* m_WindowHandle;
		inline static Ref<DebugMessenger> m_DebugMessenger;
	};

}