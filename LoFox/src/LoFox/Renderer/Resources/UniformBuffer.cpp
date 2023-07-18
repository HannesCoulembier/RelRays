#include "lfpch.h"
#include "LoFox/Renderer/Resources/UniformBuffer.h"

#include "LoFox/Renderer/Renderer.h"
#include "LoFox/Renderer/RenderContext.h"

namespace LoFox {

	UniformBuffer::UniformBuffer(uint32_t bufferSize)
		: m_Size(bufferSize) {

		m_Buffers.resize(Renderer::MaxFramesInFlight);
		m_BuffersMapped.resize(Renderer::MaxFramesInFlight);

		for (size_t i = 0; i < Renderer::MaxFramesInFlight; i++) {

			m_Buffers[i] = CreateRef<Buffer>(m_Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			vkMapMemory(RenderContext::LogicalDevice, m_Buffers[i]->GetMemory(), 0, m_Size, 0, &m_BuffersMapped[i]);
		}
	}

	void UniformBuffer::SetData(const void* data) {

		memcpy(m_BuffersMapped[Renderer::GetCurrentFrame()], data, m_Size);
	}

	std::vector<VkDescriptorBufferInfo> UniformBuffer::GetDescriptorInfos() {

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

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t bufferSize) {
		return CreateRef<UniformBuffer>(bufferSize);
	}

	void UniformBuffer::Destroy(){

		for (auto buffer : m_Buffers)
			buffer->Destroy();
	}
}