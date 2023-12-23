#pragma once

namespace LoFox {

	class Texture {

	public:
		virtual void Destroy() = 0;

		void* GetData() { return m_Data; }
		uint32_t GetWidth() { return m_Width; }
		uint32_t GetHeight() { return m_Height; }

		static Ref<Texture> Create(const std::string& path);
	protected:
		Texture(const std::string& path);
	protected:
		void* m_Data = nullptr;

		std::string m_Path;
		uint32_t m_Width, m_Height;
		uint32_t m_Size;
	};
}