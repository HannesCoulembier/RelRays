#include "lfpch.h"
#include "LoFox/Renderer/VertexBuffer.h"

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/VulkanVertexBuffer.h"
#endif

namespace LoFox {

	uint32_t GetVertexAttributeTypeSize(VertexAttributeType type) {
	
		switch (type) {
			case VertexAttributeType::Float:	return 4 * 1;
			case VertexAttributeType::Float2:	return 4 * 2;
			case VertexAttributeType::Float3:	return 4 * 3;
			case VertexAttributeType::Float4:	return 4 * 4;

			case VertexAttributeType::Mat3:		return 4 * 3 * 3;
			case VertexAttributeType::Mat4:		return 4 * 4 * 4;

			case VertexAttributeType::Int:		return 4 * 1;
			case VertexAttributeType::Int2:		return 4 * 2;
			case VertexAttributeType::Int3:		return 4 * 3;
			case VertexAttributeType::Int4:		return 4 * 4;

			case VertexAttributeType::Uint:		return 4 * 1;
			case VertexAttributeType::Bool:		return 1;
		}
	
		LF_CORE_ASSERT(false, "Unknown vertex attribute type!");
		return 0;
	}

	uint32_t GetVertexAttributeComponentCount(VertexAttributeType attribute) {

		switch (attribute) {

			case VertexAttributeType::Float:	return 1;
			case VertexAttributeType::Float2:	return 2;
			case VertexAttributeType::Float3:	return 3;
			case VertexAttributeType::Float4:	return 4;

			case VertexAttributeType::Mat3:		return 3; // 3 * float3
			case VertexAttributeType::Mat4:		return 4; // 4 * float4

			case VertexAttributeType::Int:		return 1;
			case VertexAttributeType::Int2:		return 2;
			case VertexAttributeType::Int3:		return 3;
			case VertexAttributeType::Int4:		return 4;

			case VertexAttributeType::Uint:		return 1;
			case VertexAttributeType::Bool:		return 1;
		}

		LF_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	VertexLayout::VertexLayout(std::initializer_list<VertexAttributeType> attributes)
		: m_Attributes(attributes) {

		VertexSize = 0;
		for (auto attribute : m_Attributes)
			VertexSize += GetVertexAttributeTypeSize(attribute);
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t bufferSize, const void* data, VertexLayout layout) {

		#ifdef LF_RENDERAPI_OPENGL
			return CreateRef<OpenGLVertexBuffer>(bufferSize, data, layout);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			return CreateRef<VulkanVertexBuffer>(bufferSize, data, layout);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	// VkVertexInputBindingDescription VertexLayout::GetBindingDescription() {
	// 
	// 	VkVertexInputBindingDescription bindingDescription = {};
	// 	bindingDescription.binding = m_Binding;
	// 	bindingDescription.stride = m_Size;
	// 	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	// 
	// 	return bindingDescription;
	// }

	// std::vector<VkVertexInputAttributeDescription> VertexLayout::GetAttributeDescriptions() {
	// 
	// 	std::vector<VkVertexInputAttributeDescription> attributeDescriptions(m_Attributes.size());
	// 	uint32_t offset = 0;
	// 	for (size_t i = 0; i < m_Attributes.size(); i++) {
	// 
	// 		attributeDescriptions[i].binding = m_Binding;
	// 		attributeDescriptions[i].location = i;
	// 		attributeDescriptions[i].format = VertexAttributeTypeToVkFormat(m_Attributes[i]);
	// 		attributeDescriptions[i].offset = offset;
	// 		offset += VertexAttributeTypeSize(m_Attributes[i]);
	// 	}
	// 
	// 	return attributeDescriptions;
	// }

	VertexBuffer::VertexBuffer(uint32_t bufferSize, VertexLayout layout)
		: m_VertexLayout(layout), m_Size(bufferSize) {}

	// VertexBuffer::VertexBuffer(uint32_t bufferSize, const void* data, VertexLayout layout)
	// 	: m_Size(bufferSize), m_VertexLayout(layout) {
	// 
	// 	Ref<Buffer> stagingBuffer = CreateRef<Buffer>(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	// 	m_Buffer = CreateRef<Buffer>(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	// 
	// 	stagingBuffer->SetData(data);
	// 
	// 	Buffer::CopyBuffer(stagingBuffer, m_Buffer);
	// 	stagingBuffer->Destroy();
	// }

	// void VertexBuffer::Destroy() {
	// 
	// 	m_Buffer->Destroy();
	// }
}