#pragma once

namespace LoFox {

	class UniformBuffer {

	public:
		virtual void Destroy() = 0;

		virtual void SetData(const void* data) = 0;
		void* GetData() { return m_Data; }

		static Ref<UniformBuffer> Create(uint32_t size);
	protected:
		void* m_Data = nullptr;
	};
}
