#include "lfpch.h"
#include "LoFox/Renderer/Resources/StorageBuffer.h"

#include "LoFox/Renderer/Renderer.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace LoFox {

	StorageBuffer::StorageBuffer(uint32_t objectCount, uint32_t objectSize)
		: m_ObjectCount(objectCount), m_ObjectSize(objectSize) {

		m_Size = m_ObjectCount * objectSize;

		m_Buffers.resize(Renderer::MaxFramesInFlight);
		m_BuffersMapped.resize(Renderer::MaxFramesInFlight);

		for (size_t i = 0; i < Renderer::MaxFramesInFlight; i++) {

			m_Buffers[i] = CreateRef<Buffer>(m_Size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			vkMapMemory(VulkanContext::LogicalDevice, m_Buffers[i]->GetMemory(), 0, m_Size, 0, &m_BuffersMapped[i]);
		}
	}

	void StorageBuffer::SetData(const void* data, uint32_t count) {

		for (auto buffer : m_BuffersMapped)
			memcpy(buffer, data, count * m_ObjectSize);
	}

	Ref<StorageBuffer> StorageBuffer::Create(uint32_t bufferSize, uint32_t objectSize) {
		return CreateRef<StorageBuffer>(bufferSize, objectSize);
	}

	std::vector<VkDescriptorBufferInfo> StorageBuffer::GetDescriptorInfos() {

		std::vector<VkDescriptorBufferInfo> descriptorInfos = {};
		for (const auto& buffer : m_Buffers) {

			VkDescriptorBufferInfo bufferDescriptorInfo = {};
			bufferDescriptorInfo.buffer = buffer->GetBuffer();
			bufferDescriptorInfo.offset = 0;
			bufferDescriptorInfo.range = m_Size;

			descriptorInfos.push_back(bufferDescriptorInfo);
		}

		return descriptorInfos;
	}

	void StorageBuffer::Destroy() {

		for (auto buffer : m_Buffers)
			buffer->Destroy();
	}
}