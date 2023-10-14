#pragma once

// #include "LoFox/Core/Core.h"

// #include "LoFox/Renderer/Buffer.h"

namespace LoFox {

	enum VertexAttributeType {

		Float,
		Float2,
		Float3,
		Float4,
		Mat3,
		Mat4,
		Int,
		Int2,
		Int3,
		Int4,
		Uint,
		Bool,
	};

	uint32_t GetVertexAttributeComponentCount(VertexAttributeType attribute);

	uint32_t GetVertexAttributeTypeSize(VertexAttributeType type);

	class VertexLayout {

	public:
		VertexLayout(std::initializer_list<VertexAttributeType> attributes);

		std::vector<VertexAttributeType> GetAttributes() const { return m_Attributes; }
	public:
		uint32_t VertexSize = 0;
	private:
		std::vector<VertexAttributeType> m_Attributes;
	};

	class VertexBuffer {

	public:
		virtual void Destroy() = 0;

		inline VertexLayout GetVertexLayout() { return m_VertexLayout; }
		// inline VkBuffer GetBuffer() { return m_Buffer->GetBuffer(); }
		// inline uint32_t GetSize() { return m_Size; }
		void* GetData() { return m_Data; }

		static Ref<VertexBuffer> Create(uint32_t bufferSize, const void* data, VertexLayout layout);
	protected:
		VertexBuffer(uint32_t bufferSize, VertexLayout layout);
	protected:
		// Ref<Buffer> m_Buffer;
		void* m_Data = nullptr;

		VertexLayout m_VertexLayout;
		uint32_t m_Size;
	};
}