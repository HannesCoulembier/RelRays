#pragma once

#include "LoFox/Core/Core.h"

#include "LoFox/Renderer/Buffer.h"

namespace LoFox {

	enum VertexAttributeType {
		Float2,
		Float3,
	};

	class VertexLayout {

	public:
		VertexLayout(std::initializer_list<VertexAttributeType> attributes);

		VkVertexInputBindingDescription GetBindingDescription();
		std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
	private:
		uint32_t m_Binding = 0;
		uint32_t m_Size;
		std::vector<VertexAttributeType> m_Attributes;
	};

	class VertexBuffer {

	public:
		VertexBuffer(uint32_t size, const void* data, VertexLayout layout);
		void Destroy();

		inline VertexLayout GetVertexLayout() { return m_VertexLayout; }
		inline VkBuffer GetBuffer() { return m_Buffer->GetBuffer(); }
		inline uint32_t GetSize() { return m_Size; }
	private:
		Ref<Buffer> m_Buffer;
		VertexLayout m_VertexLayout;
		uint32_t m_Size;
	};
}