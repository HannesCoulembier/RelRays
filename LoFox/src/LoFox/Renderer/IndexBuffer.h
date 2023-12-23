#pragma once

namespace LoFox {

	class IndexBuffer {

	public:
		virtual void Destroy() = 0;

		void* GetData() { return m_Data; }
		uint32_t GetNumberOfIndices() { return m_NumberOfIndices; }

		static Ref<IndexBuffer> Create(uint32_t numberOfIndices, const uint32_t* data);
	protected:
		IndexBuffer(uint32_t numberOfIndices, const uint32_t* data);
	protected:
		void* m_Data = nullptr;
		uint32_t m_NumberOfIndices;
		uint32_t m_IndexSize = sizeof(uint32_t);
		uint32_t m_Size;
	};
}