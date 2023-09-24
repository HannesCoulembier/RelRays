#include "lfpch.h"
#include "LoFox/Renderer/Resources/Texture.h"

#include <stb_image.h>

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/Utils.h"

#include "LoFox/Renderer/RenderContext.h"

namespace LoFox {

	Texture::Texture(const std::string& path)
		: m_Path(path) {

		m_Format = VK_FORMAT_R8G8B8A8_SRGB;
		stbi_uc* pixels = stbi_load(m_Path.c_str(), &m_Width, &m_Height, nullptr, STBI_rgb_alpha);
		LF_CORE_ASSERT(pixels, "Failed to load image at: " + m_Path);

		Ref<Buffer> stagingBuffer = CreateRef<Buffer>(GetSize(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer->SetData(pixels);

		stbi_image_free(pixels);

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
		imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
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

		TransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		// Copy stagingbuffer to image
		VkCommandBuffer commandBuffer = RenderContext::BeginSingleTimeCommandBuffer();

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { (uint32_t)m_Width, (uint32_t)m_Height, 1 };

		vkCmdCopyBufferToImage(commandBuffer, stagingBuffer->GetBuffer(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		RenderContext::EndSingleTimeCommandBuffer(commandBuffer);

		TransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		m_ImageView = Image::CreateImageView(m_Image, m_Format, VK_IMAGE_ASPECT_COLOR_BIT);
		stagingBuffer->Destroy();
	}

	void Texture::TransitionLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {

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
		else {
			LF_CORE_ASSERT(false, "Unsupported layout transition!");
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		RenderContext::EndSingleTimeCommandBuffer(commandBuffer);
	}

	void Texture::Destroy() {

		vkDestroyImageView(VulkanContext::LogicalDevice, m_ImageView, nullptr);
		vkDestroyImage(VulkanContext::LogicalDevice, m_Image, nullptr);
		vkFreeMemory(VulkanContext::LogicalDevice, m_Memory, nullptr);
	}

	Ref<Texture> Texture::Create(const std::string& path) {

		return CreateRef<Texture>(path);
	}
}