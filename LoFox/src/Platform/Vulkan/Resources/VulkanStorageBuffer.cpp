#include "lfpch.h"
#include "Platform/Vulkan/Resources/VulkanStorageBuffer.h"

#include "Platform/Vulkan/VulkanContext.h"

namespace LoFox {

	VulkanStorageBuffer::VulkanStorageBuffer(uint32_t maxObjectCount, uint32_t objectSize)
		: StorageBuffer(maxObjectCount, objectSize) {

		m_Data = &m_VulkanData;

		m_Buffer = Buffer::Create(m_Size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vkMapMemory(VulkanContext::LogicalDevice, m_Buffer->GetMemory(), 0, m_Size, 0, &m_MappedBuffer);

		m_VulkanData.BufferInfo.buffer = m_Buffer->GetBuffer();
		m_VulkanData.BufferInfo.offset = 0;
		m_VulkanData.BufferInfo.range = m_Size;
	}

	void VulkanStorageBuffer::Destroy() {

		m_Buffer->Destroy();
	}

	void VulkanStorageBuffer::SetData(uint32_t objectCount, const void* data) {

		memcpy(m_MappedBuffer, data, m_ObjectSize*objectCount);
	}
}