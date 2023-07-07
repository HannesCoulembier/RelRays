#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

#include "LoFox/Core/Window.h"
#include "LoFox/Events/Event.h"
#include "LoFox/Events/RenderEvent.h"

namespace LoFox {

	class DebugMessenger;

	class Buffer;

	class RenderContext {

	public:
		RenderContext() = default;

		static void Init(Ref<Window> window);
		static void Shutdown();

		static VkCommandBuffer BeginSingleTimeCommandBuffer();
		static void EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer);
	public:
		static VkInstance Instance;
		static VkPhysicalDevice PhysicalDevice;
		static VkDevice LogicalDevice;
		static VkSurfaceKHR Surface;

		static VkQueue GraphicsQueueHandle;
		static VkQueue PresentQueueHandle;

		static VkCommandPool CommandPool;
		static VkCommandBuffer MainCommandBuffer;
	private:
		static void InitInstance();
		static void InitDevices();
	private:
		static Ref<Window> m_Window;
		static Ref<DebugMessenger> m_DebugMessenger;
	};
}