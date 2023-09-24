#pragma once

#include <vulkan/vulkan.h>

namespace LoFox {

	class RenderContext {

	public:
		static VkCommandBuffer BeginSingleTimeCommandBuffer();
		static void EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer);
	};
}