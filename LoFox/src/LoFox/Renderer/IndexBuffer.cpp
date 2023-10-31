#include "lfpch.h"
#include "IndexBuffer.h"

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/OpenGLIndexBuffer.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/VulkanIndexBuffer.h"
#endif

namespace LoFox {

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t numberOfIndices, const uint32_t* data) {

		#ifdef LF_RENDERAPI_OPENGL
			return CreateRef<OpenGLIndexBuffer>(numberOfIndices, data);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			return CreateRef<VulkanIndexBuffer>(numberOfIndices, data);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	IndexBuffer::IndexBuffer(uint32_t numberOfIndices, const uint32_t* data)
		: m_NumberOfIndices(numberOfIndices), m_Size(m_NumberOfIndices * m_IndexSize) {}

	// IndexBuffer::IndexBuffer(uint32_t bufferSize, uint32_t numberOfIndices, const void* data)
	// 	: m_Size(bufferSize), m_NumberOfIndices(numberOfIndices) {
	// 
	// 	Ref<Buffer> stagingBuffer = CreateRef<Buffer>(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	// 	m_Buffer = CreateRef<Buffer>(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	// 
	// 	stagingBuffer->SetData(data);
	// 
	// 	Buffer::CopyBuffer(stagingBuffer, m_Buffer);
	// 	stagingBuffer->Destroy();
	// }
	// 
	// void IndexBuffer::Destroy() {
	// 
	// 	m_Buffer->Destroy();
	// }
}