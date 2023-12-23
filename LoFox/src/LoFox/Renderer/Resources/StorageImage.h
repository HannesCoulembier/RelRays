#pragma once

namespace LoFox {

	class StorageImage {

	public:
		virtual void Destroy() = 0;

		void* GetData() { return m_Data; }
		virtual void* GetImTextureID() = 0;

		static Ref<StorageImage> Create(uint32_t width, uint32_t height);
	protected:
		StorageImage(uint32_t width, uint32_t height);
	protected:
		void* m_Data = nullptr;
		
		uint32_t m_Width, m_Height;
	};
}