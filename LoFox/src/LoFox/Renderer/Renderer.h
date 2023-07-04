#pragma once

#include <vulkan/vulkan.h>

#include "LoFox/Renderer/RenderContext.h"

namespace LoFox {

	class Renderer {

	public:
		static void Init(Ref<Window> window);
		static void Shutdown();

		static void StartFrame();
		static void SubmitFrame();

		static void WaitIdle() { vkDeviceWaitIdle(m_Context->LogicalDevice); }

		static inline Ref<RenderContext> GetContext() { return m_Context; }
	private:
		static void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
		static void PresentSwapChainImage(uint32_t imageIndex);
	private:
		static Ref<Window> m_Window;
		static Ref<RenderContext> m_Context;

		static uint32_t m_ThisFramesImageIndex;

		static int m_CurrentFrame;
		static const int m_MaxFramesInFlight = 2;
	};
}