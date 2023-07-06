#pragma once

#include <vulkan/vulkan.h>

#include "LoFox/Core/Window.h"

#include "LoFox/Events/RenderEvent.h"

namespace LoFox {

	class Renderer {

	public:
		static void Init(Ref<Window> window);
		static void Shutdown();

		static void StartFrame();
		static void SubmitFrame();

		static void WaitIdle();

		static void OnResize(uint32_t width, uint32_t height) { OnFramebufferResize(FramebufferResizeEvent(width, height)); }
	private:
		static void RecordCommandBuffer(VkCommandBuffer commandBuffer);

		static bool OnFramebufferResize(FramebufferResizeEvent& event);
	private:
		static Ref<Window> m_Window;

		static uint32_t m_ThisFramesImageIndex;

		static int m_CurrentFrame;
		static const int m_MaxFramesInFlight = 1;
	};
}