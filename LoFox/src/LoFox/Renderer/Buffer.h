#pragma once

#include <vulkan/vulkan.h>

namespace LoFox {

	class Buffer {

	public:
		Buffer(uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

		void Destroy();

		void SetData(const void* src);

		inline VkBuffer GetBuffer() { return m_Buffer; }
		inline VkDeviceMemory GetMemory() { return m_Memory; }
		inline uint32_t GetSize() { return m_Size; }

		static void CopyBuffer(Ref<Buffer> srcBuffer, Ref<Buffer> dstBuffer);
	private:
		VkBuffer m_Buffer;
		VkDeviceMemory m_Memory;
		uint32_t m_Size;
	};
}