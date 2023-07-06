#pragma once

#include "LoFox/Core/Core.h"

#include "LoFox/Renderer/Buffer.h"

namespace LoFox {

	class Image {

	public:
		Image(const std::string& path);
		Image(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);

		void Destroy();

		inline int GetWidth() { return m_Width; }
		inline int GetHeight() { return m_Height; }
		inline int GetSize() { return m_Width * m_Height * 4; } // 4 floats per pixel
		inline VkImageView GetImageView() { return m_ImageView; }
		inline VkFormat GetFormat() { return m_Format; }

		void TransitionLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	private:
		void CreateImageView(VkImageAspectFlags aspectFlags);
	private:
		const std::string& m_Path = "";

		VkImage m_Image;
		VkDeviceMemory m_Memory;
		VkImageView m_ImageView;

		VkFormat m_Format;

		int m_Width, m_Height;
	};
}