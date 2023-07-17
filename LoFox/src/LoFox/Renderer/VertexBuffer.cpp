#include "lfpch.h"
#include "LoFox/Renderer/VertexBuffer.h"

namespace LoFox {

	VkFormat VertexAttributeTypeToVkFormat(VertexAttributeType type) {

		switch (type) {
		case VertexAttributeType::Float2:	return VK_FORMAT_R32G32_SFLOAT;
		case VertexAttributeType::Float3:	return VK_FORMAT_R32G32B32_SFLOAT;
		}

		LF_CORE_ASSERT(false, "Unknown vertex attribute type!");
		return VK_FORMAT_UNDEFINED;
	}

	uint32_t VertexAttributeTypeSize(VertexAttributeType type) {

		switch (type) {
		case VertexAttributeType::Float2:	return 2 * 4;
		case VertexAttributeType::Float3:	return 3 * 4;
		}

		LF_CORE_ASSERT(false, "Unknown vertex attribute type!");
		return 0;
	}

	VertexLayout::VertexLayout(std::initializer_list<VertexAttributeType> attributes)
		: m_Attributes(attributes) {
		m_Size = 0;
		for (auto attribute : m_Attributes)
			m_Size += VertexAttributeTypeSize(attribute);
	}

	VkVertexInputBindingDescription VertexLayout::GetBindingDescription() {

		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = m_Binding;
		bindingDescription.stride = m_Size;
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	std::vector<VkVertexInputAttributeDescription> VertexLayout::GetAttributeDescriptions() {

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(m_Attributes.size());
		uint32_t offset = 0;
		for (size_t i = 0; i < m_Attributes.size(); i++) {

			attributeDescriptions[i].binding = m_Binding;
			attributeDescriptions[i].location = i;
			attributeDescriptions[i].format = VertexAttributeTypeToVkFormat(m_Attributes[i]);
			attributeDescriptions[i].offset = offset;
			offset += VertexAttributeTypeSize(m_Attributes[i]);
		}

		return attributeDescriptions;
	}

	VertexBuffer::VertexBuffer(uint32_t bufferSize, const void* data, VertexLayout layout)
		: m_Size(bufferSize), m_VertexLayout(layout) {

		Ref<Buffer> stagingBuffer = CreateRef<Buffer>(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_Buffer = CreateRef<Buffer>(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		stagingBuffer->SetData(data);

		Buffer::CopyBuffer(stagingBuffer, m_Buffer);
		stagingBuffer->Destroy();
	}

	void VertexBuffer::Destroy() {

		m_Buffer->Destroy();
	}
}