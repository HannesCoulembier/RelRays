#include "lfpch.h"
#include "Platform/Vulkan/VulkanIndexBuffer.h"

namespace LoFox {

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t numberOfIndices, const uint32_t* data)
		: IndexBuffer(numberOfIndices, data) {

		m_Data = &m_VulkanData;

		Ref<Buffer> stagingBuffer = Buffer::Create(m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_VulkanData.Buffer = Buffer::Create(m_Size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		
		stagingBuffer->SetData(data);
		
		Buffer::CopyBuffer(stagingBuffer, m_VulkanData.Buffer);
		stagingBuffer->Destroy();
	}

	void VulkanIndexBuffer::Destroy() {

		m_VulkanData.Buffer->Destroy();
	}
}