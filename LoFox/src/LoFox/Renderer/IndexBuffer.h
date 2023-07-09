#pragma once

#include "LoFox/Core/Core.h"

#include "LoFox/Renderer/Buffer.h"

namespace LoFox {

	class IndexBuffer {

	public:
		IndexBuffer(uint32_t size, uint32_t numberOfIndices, const void* data);
		void Destroy();

		inline VkBuffer GetBuffer() { return m_Buffer->GetBuffer(); }
		inline uint32_t GetSize() { return m_Size; }
		inline uint32_t GetNumberOfIndices() { return m_NumberOfIndices; }
	private:
		Ref<Buffer> m_Buffer;
		uint32_t m_Size;
		uint32_t m_NumberOfIndices;
	};
}