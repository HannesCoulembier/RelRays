#pragma once
#include "LoFox/Renderer/Resources/StorageImage.h"

#include <vulkan/vulkan.h>

#include "Platform/Vulkan/Image.h"

namespace LoFox {

	struct VulkanStorageImageData {

		VkDescriptorImageInfo ImageInfo;
	};

	class VulkanStorageImage : public StorageImage {

	public:
		VulkanStorageImage(uint32_t width, uint32_t height);
		virtual void Destroy() override;
	private:
		VulkanStorageImageData m_VulkanData;

		VkFormat m_Format;
		Ref<Image> m_Image;
		VkSampler m_Sampler;
	};
}