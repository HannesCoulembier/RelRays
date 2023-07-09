#include "lfpch.h"
#include "VertexBuffer.h"

namespace LoFox {

	VertexBuffer::VertexBuffer(uint32_t bufferSize, const void* data)
		: m_Size(bufferSize) {

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