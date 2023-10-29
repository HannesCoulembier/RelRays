#pragma once

namespace LoFox {

	class Texture {

	public:
		virtual void Destroy() = 0;

		void* GetData() { return m_Data; }

		static Ref<Texture> Create();
	protected:
		void* m_Data = nullptr;
	};
}