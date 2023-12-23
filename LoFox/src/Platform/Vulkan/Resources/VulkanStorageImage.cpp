#include "lfpch.h"
#include "Platform/Vulkan/Resources/VulkanStorageImage.h"

#include <backends/imgui_impl_vulkan.h>

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/Buffer.h"
#include "Platform/Vulkan/Utils.h"

namespace LoFox {

	VulkanStorageImage::VulkanStorageImage(uint32_t width, uint32_t height)
		: StorageImage(width, height) {

		m_Data = &m_VulkanData;

		m_Format = VK_FORMAT_R8G8B8A8_UNORM;

		m_Image = Image::Create(m_Format, m_Width, m_Height, 1, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

		// 		TransitionLayout(VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

		VulkanContext::ImmediateSubmit([&](VkCommandBuffer cmd) {

			// Specify what range to change the layout of
			VkImageSubresourceRange range = {};
			range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			range.baseArrayLayer = 0;
			range.baseMipLevel = 0;
			range.layerCount = 1;
			range.levelCount = 1;

			// Transition image layout to be readable by shader
			VkImageMemoryBarrier imageBarrierToReadable = {};
			imageBarrierToReadable.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
			// imageBarrierToReadable.dstQueueFamilyIndex = 
			imageBarrierToReadable.image = m_Image->GetImage();
			imageBarrierToReadable.newLayout = VK_IMAGE_LAYOUT_GENERAL;
			imageBarrierToReadable.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageBarrierToReadable.pNext = nullptr;
			imageBarrierToReadable.srcAccessMask = 0;
			// imageBarrierToReadable.srcQueueFamilyIndex = 
			imageBarrierToReadable.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageBarrierToReadable.subresourceRange = range;

			vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageBarrierToReadable);
		});

		VkSamplerCreateInfo samplerCreateInfo = Utils::MakeSamplerCreateInfo(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT); // TODO: give user control over these variables
		vkCreateSampler(VulkanContext::LogicalDevice, &samplerCreateInfo, nullptr, &m_Sampler);

		m_VulkanData.ImageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		m_VulkanData.ImageInfo.imageView = m_Image->GetImageView();
		m_VulkanData.ImageInfo.sampler = m_Sampler;

		m_DescriptorSet = ImGui_ImplVulkan_AddTexture(m_Sampler, m_VulkanData.ImageInfo.imageView, VK_IMAGE_LAYOUT_GENERAL);
	}

	void VulkanStorageImage::Destroy() {

		vkDestroySampler(VulkanContext::LogicalDevice, m_Sampler, nullptr);
		m_Image->Destroy();
	}

	void* VulkanStorageImage::GetImTextureID() {

		return &m_DescriptorSet;
	}
}