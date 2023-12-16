#include "lfpch.h"
#include "Platform/Vulkan/VulkanFramebuffer.h"

#include <backends/imgui_impl_vulkan.h>

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/Utils.h"

namespace LoFox {

	namespace Utils {

		VkFormat LoFoxFBTextureFormatToVkFormat(FramebufferTextureFormat format) {

			switch (format) {
				case FramebufferTextureFormat::None: LF_CORE_ASSERT(false, "Cannot create framebuffer attachment of format None!");
				// case FramebufferTextureFormat::RGBA8: return VK_FORMAT_R8G8B8A8_SRGB;
				// case FramebufferTextureFormat::RGBA8: return VK_FORMAT_B8G8R8A8_SRGB;
				case FramebufferTextureFormat::RGBA8: return VK_FORMAT_B8G8R8A8_UNORM;
			}

			LF_CORE_ASSERT(false, "Unknown FramebufferTextureFormat!");
		}
	}

	VulkanFramebuffer::VulkanFramebuffer(uint32_t width, uint32_t height, VkImageView imageView, VkRenderPass renderPass) {

		FramebufferCreateInfo createInfo = {};
		createInfo.Height = height;
		createInfo.Width = width;
		createInfo.SwapChainTarget = true; // This means one should not use the framebuffer as an image
		m_CreateInfo = createInfo;

		m_Data = &m_VulkanData;

		m_VulkanData.AttachmentViews.push_back(imageView);

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.attachmentCount = m_VulkanData.AttachmentViews.size();
		framebufferInfo.flags = 0;
		framebufferInfo.height = m_CreateInfo.Height;
		framebufferInfo.layers = 1;
		framebufferInfo.pAttachments = m_VulkanData.AttachmentViews.data();
		framebufferInfo.pNext = nullptr;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.width = m_CreateInfo.Width;

		LF_CORE_ASSERT(vkCreateFramebuffer(VulkanContext::LogicalDevice, &framebufferInfo, nullptr, &m_VulkanData.Framebuffer) == VK_SUCCESS, "Failed to create framebuffers!");

		VkSamplerCreateInfo samplerCreateInfo = Utils::MakeSamplerCreateInfo(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT); // TODO: give user control over these variables
		vkCreateSampler(VulkanContext::LogicalDevice, &samplerCreateInfo, nullptr, &m_Sampler);
	}

	VulkanFramebuffer::VulkanFramebuffer(FramebufferCreateInfo createInfo)
		: Framebuffer(createInfo) {

		m_Data = &m_VulkanData;

		for (FramebufferTextureDescription attachmentDescription : m_CreateInfo.Attachments) {

			Ref<Image> attachment = Image::Create(Utils::LoFoxFBTextureFormatToVkFormat(attachmentDescription.TextureFormat), m_CreateInfo.Width, m_CreateInfo.Height, 1, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
			VkImageView view = attachment->GetImageView();
			m_VulkanData.AttachmentViews.push_back(view);
			m_VulkanData.Attachments.push_back(attachment);
		}

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.attachmentCount = m_VulkanData.AttachmentViews.size();
		// framebufferInfo.flags = 
		framebufferInfo.height = m_CreateInfo.Height;
		framebufferInfo.layers = 1;
		framebufferInfo.pAttachments = m_VulkanData.AttachmentViews.data();
		// framebufferInfo.pNext = 
		framebufferInfo.renderPass = VulkanContext::RenderPass;
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.width = m_CreateInfo.Width;

		LF_CORE_ASSERT(vkCreateFramebuffer(VulkanContext::LogicalDevice, &framebufferInfo, nullptr, &m_VulkanData.Framebuffer) == VK_SUCCESS, "Failed to create framebuffers!");
	
		VkSamplerCreateInfo samplerCreateInfo = Utils::MakeSamplerCreateInfo(VK_FILTER_NEAREST, VK_SAMPLER_ADDRESS_MODE_REPEAT); // TODO: give user control over these variables
		vkCreateSampler(VulkanContext::LogicalDevice, &samplerCreateInfo, nullptr, &m_Sampler);

		for (int i = 0; i < m_VulkanData.AttachmentViews.size(); i++)
			m_DescriptorSets.push_back(ImGui_ImplVulkan_AddTexture(m_Sampler, m_VulkanData.AttachmentViews[i], VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL));
	}

	Ref<VulkanFramebuffer> VulkanFramebuffer::CreateFromImageView(uint32_t width, uint32_t height, VkImageView imageView, VkRenderPass renderPass) {

		return CreateRef<VulkanFramebuffer>(width, height, imageView, renderPass);
	}

	void VulkanFramebuffer::Destroy() {

		vkDestroyFramebuffer(VulkanContext::LogicalDevice, m_VulkanData.Framebuffer, nullptr);
		if (!m_CreateInfo.SwapChainTarget) // If it is a SwapChainTarget, m_VulkanData.Attachments contains no images as there are only image views
			for (auto attachment : m_VulkanData.Attachments)
				attachment->Destroy();
		vkDestroySampler(VulkanContext::LogicalDevice, m_Sampler, nullptr);
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height) {

	}

	void* VulkanFramebuffer::GetAttachmentImTextureID(uint32_t index) {

		return &m_DescriptorSets[index];
	}
}