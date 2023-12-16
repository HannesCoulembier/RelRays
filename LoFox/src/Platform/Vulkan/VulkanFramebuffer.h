#pragma once

#include "LoFox/Renderer/Framebuffer.h"

#include <vulkan/vulkan.h>

#include "Platform/Vulkan/Image.h"

namespace LoFox {

	struct VulkanFramebufferData {

		VkFramebuffer Framebuffer;
		std::vector<Ref<Image>> Attachments;
		std::vector<VkImageView> AttachmentViews;
		// GLuint RendererID = 0;
		// std::vector<GLuint> Attachments;
	};

	class VulkanFramebuffer : public Framebuffer {

	public:
		VulkanFramebuffer(FramebufferCreateInfo createInfo);
		VulkanFramebuffer(uint32_t width, uint32_t height, VkImageView imageView, VkRenderPass renderPass);

		virtual void Resize(uint32_t width, uint32_t height) override;
		virtual void Destroy() override;

		virtual void* GetAttachmentImTextureID(uint32_t index) override;

		static Ref<VulkanFramebuffer> CreateFromImageView(uint32_t width, uint32_t height, VkImageView imageView, VkRenderPass renderPass);
	private:
		VulkanFramebufferData m_VulkanData;
		VkSampler m_Sampler;

		// ImGui data
		std::vector<VkDescriptorSet> m_DescriptorSets;
	};
}