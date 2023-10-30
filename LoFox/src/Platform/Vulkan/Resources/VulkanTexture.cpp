#include "lfpch.h"
#include "Platform/Vulkan/Resources/VulkanTexture.h"

#include <stb_image.h>

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Utils.h"

namespace LoFox {

	VulkanTexture::VulkanTexture(const std::string& path)
		: Texture(path) {

		m_Data = &m_VulkanData;
		
		m_Format = VK_FORMAT_R8G8B8A8_SRGB;

		int width, height;
		stbi_set_flip_vertically_on_load(true);
		stbi_uc* pixels = stbi_load(m_Path.c_str(), &width, &height, nullptr, STBI_rgb_alpha);
		LF_CORE_ASSERT(pixels, "Failed to load image at: " + m_Path);
		m_Width = (uint32_t)width;
		m_Height = (uint32_t)height;
		m_Size = m_Width * m_Height * 4; // 4 floats per pixel

		Ref<Buffer> stagingBuffer = Buffer::Create(m_Size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		stagingBuffer->SetData(pixels);

		stbi_image_free(pixels);

		m_Image = Image::Create(m_Format, m_Width, m_Height, 1, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

		VulkanContext::ImmediateSubmit([&](VkCommandBuffer cmd) {

			// Specify what range to change the layout of
			VkImageSubresourceRange range = {};
			range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			range.baseArrayLayer = 0;
			range.baseMipLevel = 0;
			range.layerCount = 1;
			range.levelCount = 1;

			// Transition image layout to copy stagingbuffer into
			VkImageMemoryBarrier imageBarrierToTransfer = {};
			imageBarrierToTransfer.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			// imageBarrierToTransfer.dstQueueFamilyIndex = 
			imageBarrierToTransfer.image = m_Image->GetImage();
			imageBarrierToTransfer.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrierToTransfer.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageBarrierToTransfer.pNext = nullptr;
			imageBarrierToTransfer.srcAccessMask = 0;
			// imageBarrierToTransfer.srcQueueFamilyIndex = 
			imageBarrierToTransfer.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrierToTransfer.subresourceRange = range;

			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrierToTransfer);

			VkBufferImageCopy copyRegion = {};
			copyRegion.bufferImageHeight = 0;
			copyRegion.bufferOffset = 0;
			copyRegion.bufferRowLength = 0;
			copyRegion.imageExtent = m_Image->GetExtent();
			// copyRegion.imageOffset = 
			copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			copyRegion.imageSubresource.baseArrayLayer = 0;
			copyRegion.imageSubresource.layerCount = 1;
			copyRegion.imageSubresource.mipLevel = 0;

			vkCmdCopyBufferToImage(cmd, stagingBuffer->GetBuffer(), m_Image->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

			// Transition image layout to be readable by shader
			VkImageMemoryBarrier imageBarrierToReadable = {};
			imageBarrierToReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			// imageBarrierToReadable.dstQueueFamilyIndex = 
			imageBarrierToReadable.image = m_Image->GetImage();
			imageBarrierToReadable.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageBarrierToReadable.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageBarrierToReadable.pNext = nullptr;
			imageBarrierToReadable.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			// imageBarrierToReadable.srcQueueFamilyIndex = 
			imageBarrierToReadable.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrierToReadable.subresourceRange = range;

			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrierToReadable);
		});

		stagingBuffer->Destroy();

		VkSamplerCreateInfo samplerCreateInfo = Utils::MakeSamplerCreateInfo(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT); // TODO: give user control over these variables
		vkCreateSampler(VulkanContext::LogicalDevice, &samplerCreateInfo, nullptr, &m_Sampler);

		m_VulkanData.ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		m_VulkanData.ImageInfo.imageView = m_Image->GetImageView();
		m_VulkanData.ImageInfo.sampler = m_Sampler;
	}

	void VulkanTexture::Destroy() {

		vkDestroySampler(VulkanContext::LogicalDevice, m_Sampler, nullptr);
		m_Image->Destroy();
	}
}