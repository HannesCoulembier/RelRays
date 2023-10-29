#include "lfpch.h"
#include "Platform/Vulkan/Resources/VulkanUniformBuffer.h"

#include "Platform/Vulkan/VulkanContext.h"

namespace LoFox {

	VulkanUniformBuffer::VulkanUniformBuffer(uint32_t size)
		: m_Size(size) {

		m_Data = &m_VulkanData;

		m_Buffer = Buffer::Create(m_Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		vkMapMemory(VulkanContext::LogicalDevice, m_Buffer->GetMemory(), 0, m_Size, 0, &m_MappedBuffer);
	
		m_VulkanData.BufferInfo.buffer = m_Buffer->GetBuffer();
		m_VulkanData.BufferInfo.offset = 0;
		m_VulkanData.BufferInfo.range = m_Size;
	}

	void VulkanUniformBuffer::Destroy() {

		m_Buffer->Destroy();
	}

	void VulkanUniformBuffer::SetData(const void* data) {

		memcpy(m_MappedBuffer, data, m_Size);
	}
}