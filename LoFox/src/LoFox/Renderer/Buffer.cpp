#include "lfpch.h"
#include "LoFox/Renderer/Buffer.h"

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/Utils.h"

#include "LoFox/Renderer/RenderContext.h"

namespace LoFox {

	Buffer::Buffer(uint32_t size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
		: m_Size(size) {

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = m_Size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		LF_CORE_ASSERT(vkCreateBuffer(VulkanContext::LogicalDevice, &bufferInfo, nullptr, &m_Buffer) == VK_SUCCESS, "Failed to create buffer!");

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(VulkanContext::LogicalDevice, m_Buffer, &memRequirements);

		VkMemoryAllocateInfo memoryAllocInfo = {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memRequirements.size;
		memoryAllocInfo.memoryTypeIndex = Utils::FindMemoryType(VulkanContext::PhysicalDevice, memRequirements.memoryTypeBits, properties);

		LF_CORE_ASSERT(vkAllocateMemory(VulkanContext::LogicalDevice, &memoryAllocInfo, nullptr, &m_Memory) == VK_SUCCESS, "Failed to allocate buffer memory!");

		vkBindBufferMemory(VulkanContext::LogicalDevice, m_Buffer, m_Memory, 0);
	}

	void Buffer::SetData(const void* src) {

		void* data;
		vkMapMemory(VulkanContext::LogicalDevice, m_Memory, 0, m_Size, 0, &data);
		memcpy(data, src, (size_t)m_Size);
		vkUnmapMemory(VulkanContext::LogicalDevice, m_Memory);
	}

	void Buffer::Destroy() {

		vkDestroyBuffer(VulkanContext::LogicalDevice, m_Buffer, nullptr);
		vkFreeMemory(VulkanContext::LogicalDevice, m_Memory, nullptr);
	}

	void Buffer::CopyBuffer(Ref<Buffer> srcBuffer, Ref<Buffer> dstBuffer) {

		VkCommandBuffer commandBuffer = RenderContext::BeginSingleTimeCommandBuffer();

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = dstBuffer->GetSize();
		vkCmdCopyBuffer(commandBuffer, srcBuffer->GetBuffer(), dstBuffer->GetBuffer(), 1, &copyRegion);

		RenderContext::EndSingleTimeCommandBuffer(commandBuffer);
	}
}