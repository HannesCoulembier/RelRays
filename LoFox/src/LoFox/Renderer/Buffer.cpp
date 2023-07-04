#include "lfpch.h"
#include "LoFox/Renderer/Buffer.h"

#include "LoFox/Utils/VulkanUtils.h"

namespace LoFox {

	Buffer::Buffer(Ref<RenderContext> context, uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
		: m_Context(context), m_Size(size) {

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = m_Size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		LF_CORE_ASSERT(vkCreateBuffer(m_Context->GetLogicalDevice(), &bufferInfo, nullptr, &m_Buffer) == VK_SUCCESS, "Failed to create buffer!");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_Context->GetLogicalDevice(), m_Buffer, &memRequirements);

		VkMemoryAllocateInfo memoryAllocInfo = {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memRequirements.size;
		memoryAllocInfo.memoryTypeIndex = Utils::FindMemoryType(m_Context->GetPhysicalDevice(), memRequirements.memoryTypeBits, properties);

		LF_CORE_ASSERT(vkAllocateMemory(m_Context->GetLogicalDevice(), &memoryAllocInfo, nullptr, &m_Memory) == VK_SUCCESS, "Failed to allocate buffer memory!");

		vkBindBufferMemory(m_Context->GetLogicalDevice(), m_Buffer, m_Memory, 0);
	}

	void Buffer::SetData(const void* src) {

		void* data;
		vkMapMemory(m_Context->GetLogicalDevice(), m_Memory, 0, m_Size, 0, &data);
		memcpy(data, src, (size_t)m_Size);
		vkUnmapMemory(m_Context->GetLogicalDevice(), m_Memory);
	}

	Buffer::~Buffer() {

		vkDestroyBuffer(m_Context->GetLogicalDevice(), m_Buffer, nullptr);
		vkFreeMemory(m_Context->GetLogicalDevice(), m_Memory, nullptr);
	}
}