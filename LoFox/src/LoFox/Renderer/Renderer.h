#pragma once

#include <vulkan/vulkan.h>

#include "LoFox/Renderer/RenderContext.h"

namespace LoFox {

	class Renderer {

	public:
		static void Init(Ref<Window> window);
		static void Shutdown();

		static void WaitIdle() { vkDeviceWaitIdle(m_Context->LogicalDevice); }

		static inline Ref<RenderContext> GetContext() { return m_Context; }
	private:
		static Ref<Window> m_Window;
		static Ref<RenderContext> m_Context;
	};
}