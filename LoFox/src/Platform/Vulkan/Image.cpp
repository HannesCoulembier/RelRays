#include "lfpch.h"
#include "Platform/Vulkan/Image.h"

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/Utils.h"

namespace LoFox {

	Ref<Image> Image::Create(VkFormat format, uint32_t width, uint32_t height, uint32_t depth, VkImageUsageFlags usage) {

		return CreateRef<Image>(format, width, height, depth, usage);
	}

	Image::Image(VkFormat format, uint32_t width, uint32_t height, uint32_t depth, VkImageUsageFlags usage)
		: m_Format(format), m_Width(width), m_Height(height), m_Depth(depth) {

		m_Extent.width = m_Width;
		m_Extent.height = m_Height;
		m_Extent.depth = m_Depth;

		VkImageCreateInfo createInfo = {};
		createInfo.arrayLayers = 1;
		createInfo.extent = m_Extent;
		createInfo.flags = 0;
		createInfo.format = m_Format;
		createInfo.imageType = (m_Depth == 1) ? ((m_Height == 1) ? VK_IMAGE_TYPE_1D : VK_IMAGE_TYPE_2D) : VK_IMAGE_TYPE_3D;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		createInfo.mipLevels = 1;
		createInfo.pNext = nullptr;
		// createInfo.pQueueFamilyIndices = 
		// createInfo.queueFamilyIndexCount = 
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		createInfo.usage = usage;

		vkCreateImage(VulkanContext::LogicalDevice, &createInfo, nullptr, &m_Image);

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(VulkanContext::LogicalDevice, m_Image, &memRequirements);

		VkMemoryAllocateInfo memoryAllocInfo = {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memRequirements.size;
		memoryAllocInfo.memoryTypeIndex = Utils::FindMemoryType(VulkanContext::PhysicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		
		LF_CORE_ASSERT(vkAllocateMemory(VulkanContext::LogicalDevice, &memoryAllocInfo, nullptr, &m_Memory) == VK_SUCCESS, "Failed to allocate image memory!");
		
		vkBindImageMemory(VulkanContext::LogicalDevice, m_Image, m_Memory, 0);

		m_ImageView = Utils::CreateImageViewFromImage(VulkanContext::LogicalDevice, m_Image, m_Format, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void Image::Destroy() {

		vkFreeMemory(VulkanContext::LogicalDevice, m_Memory, nullptr);
		vkDestroyImageView(VulkanContext::LogicalDevice, m_ImageView, nullptr);
		vkDestroyImage(VulkanContext::LogicalDevice, m_Image, nullptr);
	}
}