#pragma once

#include <vulkan/vulkan.h>

#include "LoFox/Renderer/RenderContext.h"

namespace LoFox {

	class Buffer {

	public:
		Buffer(Ref<RenderContext> context, uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
		~Buffer() { Destroy(); }

		void Destroy();

		void SetData(const void* src);

		inline VkBuffer GetBuffer() { return m_Buffer; }
		inline VkDeviceMemory GetMemory() { return m_Memory; }
		inline uint32_t GetSize() { return m_Size; }
	private:
		Ref<RenderContext> m_Context;

		VkBuffer m_Buffer;
		VkDeviceMemory m_Memory;
		uint32_t m_Size;
	};
}