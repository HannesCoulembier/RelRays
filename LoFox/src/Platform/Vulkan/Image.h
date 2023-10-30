#pragma once

#include <vulkan/vulkan.h>

namespace LoFox {

	class Image {

	public:
		void Destroy();

		inline VkImage GetImage() { return m_Image; }
		inline VkExtent3D GetExtent() { return m_Extent; }
		inline VkImageView GetImageView() { return m_ImageView; }

		static Ref<Image> Create(VkFormat format, uint32_t width, uint32_t height, uint32_t depth, VkImageUsageFlags usage);
	public:
		Image(VkFormat format, uint32_t width, uint32_t height, uint32_t depth, VkImageUsageFlags usage);
	private:
		VkFormat m_Format;
		VkExtent3D m_Extent;
		uint32_t m_Width, m_Height, m_Depth;

		VkImage m_Image;
		VkDeviceMemory m_Memory;
		VkImageView m_ImageView;
	};
}