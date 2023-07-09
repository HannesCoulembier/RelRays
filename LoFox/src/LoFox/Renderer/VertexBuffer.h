#pragma once

#include "LoFox/Core/Core.h"

#include "LoFox/Renderer/Buffer.h"

namespace LoFox {

	class VertexBuffer {

	public:
		VertexBuffer(uint32_t size, const void* data);
		void Destroy();

		inline VkBuffer GetBuffer() { return m_Buffer->GetBuffer(); }
		inline uint32_t GetSize() { return m_Size; }
	private:
		Ref<Buffer> m_Buffer;
		uint32_t m_Size;
	};
}