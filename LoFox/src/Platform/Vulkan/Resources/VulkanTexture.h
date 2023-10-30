#pragma once
#include "LoFox/Renderer/Resources/Texture.h"

#include <vulkan/vulkan.h>

#include "Platform/Vulkan/Image.h"

namespace LoFox {

	struct VulkanTextureData {

		VkDescriptorImageInfo ImageInfo;
	};

	class VulkanTexture : public Texture {

	public:
		VulkanTexture(const std::string& path);
		virtual void Destroy() override;
	private:
		VulkanTextureData m_VulkanData;

		VkFormat m_Format;
		Ref<Image> m_Image;
		VkSampler m_Sampler;
	};
}