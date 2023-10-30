#pragma once

namespace LoFox {

	class Texture {

	public:
		virtual void Destroy() = 0;

		void* GetData() { return m_Data; }

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

// #pragma once
// 
// #include <vulkan/vulkan.h>
// 
// #include "LoFox/Renderer/Image.h"
// 
// namespace LoFox {
// 
// 	class Texture {
// 
// 	public:
// 		Texture(const std::string& path);
// 		void Destroy();
// 
// 		inline int GetWidth() { return m_Width; }
// 		inline int GetHeight() { return m_Height; }
// 		inline int GetSize() { return m_Width * m_Height * 4; } // 4 floats per pixel
// 		inline VkImageView GetImageView() { return m_ImageView; }
// 		inline VkFormat GetFormat() { return m_Format; }
// 
// 		void TransitionLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
// 
// 		static Ref<Texture> Create(const std::string& path);
// 	private:
// 		std::string m_Path;
// 
// 		VkImage m_Image;
// 		VkDeviceMemory m_Memory;
// 		VkImageView m_ImageView;
// 
// 		VkFormat m_Format;
// 
// 		int m_Width, m_Height;
// 	};
// }