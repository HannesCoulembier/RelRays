#include "lfpch.h"
#include "LoFox/Renderer/Resources/StorageImage.h"

#include "LoFox/Renderer/Image.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/Utils.h"

#include "LoFox/Renderer/Renderer.h"
#include "LoFox/Renderer/RenderContext.h"

namespace LoFox {

	StorageImage::StorageImage(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height) {

		m_Format = VK_FORMAT_R8G8B8A8_UNORM;

		VkImageCreateInfo imageCreateInfo = {};
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.extent.width = static_cast<uint32_t>(m_Width);
		imageCreateInfo.extent.height = static_cast<uint32_t>(m_Height);
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.format = m_Format;
		imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageCreateInfo.usage = VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.flags = 0;

		LF_CORE_ASSERT(vkCreateImage(VulkanContext::LogicalDevice, &imageCreateInfo, nullptr, &m_Image) == VK_SUCCESS, "Failed to create image!");

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(VulkanContext::LogicalDevice, m_Image, &memRequirements);

		VkMemoryAllocateInfo memoryAllocInfo = {};
		memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocInfo.allocationSize = memRequirements.size;
		memoryAllocInfo.memoryTypeIndex = Utils::FindMemoryType(VulkanContext::PhysicalDevice, memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		LF_CORE_ASSERT(vkAllocateMemory(VulkanContext::LogicalDevice, &memoryAllocInfo, nullptr, &m_Memory) == VK_SUCCESS, "Failed to allocate image memory!");

		vkBindImageMemory(VulkanContext::LogicalDevice, m_Image, m_Memory, 0);

		TransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		m_ImageView = Image::CreateImageView(m_Image, m_Format, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	VkDescriptorImageInfo StorageImage::GetDescriptorInfo() {

		VkDescriptorImageInfo imageDescriptorInfo;
		
		imageDescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		imageDescriptorInfo.imageView = m_ImageView;
		imageDescriptorInfo.sampler = Renderer::GetImageSampler();

		return imageDescriptorInfo;
	}

	void StorageImage::Destroy() {

		vkDestroyImageView(VulkanContext::LogicalDevice, m_ImageView, nullptr);
		vkDestroyImage(VulkanContext::LogicalDevice, m_Image, nullptr);
		vkFreeMemory(VulkanContext::LogicalDevice, m_Memory, nullptr);
	}

	void StorageImage::TransitionLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		VkCommandBuffer commandBuffer = RenderContext::BeginSingleTimeCommandBuffer();

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = m_Image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_GENERAL) {

			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		}
		else {
			LF_CORE_ASSERT(false, "Unsupported layout transition!");
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		RenderContext::EndSingleTimeCommandBuffer(commandBuffer);
	}

	Ref<StorageImage> StorageImage::Create(uint32_t width, uint32_t height) {

		return CreateRef<StorageImage>(width, height);
	}
}