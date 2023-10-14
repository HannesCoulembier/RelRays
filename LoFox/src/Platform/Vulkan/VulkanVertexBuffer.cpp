#include "lfpch.h"
#include "Platform/Vulkan/VulkanVertexBuffer.h"

#include "LoFox/Renderer/VertexBuffer.h"

namespace LoFox {

	namespace Utils {

		VkFormat VertexAttributeTypeToVkFormat(VertexAttributeType type) {
		
			switch (type) {
				case VertexAttributeType::Float2:	return VK_FORMAT_R32G32_SFLOAT;
				case VertexAttributeType::Float3:	return VK_FORMAT_R32G32B32_SFLOAT;
				case VertexAttributeType::Uint:		return VK_FORMAT_R32_UINT;
			}
		
			LF_CORE_ASSERT(false, "Unknown vertex attribute type!");
			return VK_FORMAT_UNDEFINED;
		}

		VkVertexInputBindingDescription VertexLayoutToVertexInputBindingDescription(uint32_t binding, const VertexLayout& layout) {

			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = binding;
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			bindingDescription.stride = layout.VertexSize;

			return bindingDescription;
		}

		void AddVertexInputAttributeDescriptionsFromVertexLayout(uint32_t binding, const VertexLayout& layout, std::vector<VkVertexInputAttributeDescription>& attributes) {

			VkVertexInputAttributeDescription attributeDescription = {};

			uint32_t offset = 0;
			std::vector<VertexAttributeType> attributeTypes = layout.GetAttributes();
			for (size_t i = 0; i < attributeTypes.size(); i++) {

				attributeDescription.binding = binding;
				attributeDescription.format = VertexAttributeTypeToVkFormat(attributeTypes[i]);
				attributeDescription.location = i;
				attributeDescription.offset = offset;
				offset += GetVertexAttributeTypeSize(attributeTypes[i]);

				attributes.push_back(attributeDescription);
			}
		}
	}

	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t bufferSize, const void* data, VertexLayout layout)
		: VertexBuffer(bufferSize, layout) {

		m_Data = &m_VulkanData;

		Ref<Buffer> stagingBuffer = Buffer::Create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_VulkanData.Buffer = Buffer::Create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		
		stagingBuffer->SetData(data);
		
		Buffer::CopyBuffer(stagingBuffer, m_VulkanData.Buffer);
		stagingBuffer->Destroy();
	}

	void VulkanVertexBuffer::Destroy() {
	
		m_VulkanData.Buffer->Destroy();
	}
}