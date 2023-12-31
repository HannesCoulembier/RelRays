#pragma once
#include "LoFox/Renderer/Resource.h"

#include <vulkan/vulkan.h>

namespace LoFox {

	VkDescriptorType ResourceTypeToVulkanDescriptorType(ResourceType type);
	std::vector<VkDescriptorBufferInfo> GetVkDescriptorBufferInfoFromResource(Resource resource);
	std::vector<VkDescriptorImageInfo> GetVkDescriptorImageInfoFromResource(Resource resource);

	struct VulkanResourceLayoutData {

		VkDescriptorSetLayout DescriptorSetLayout;
	};

	class VulkanResourceLayout : public ResourceLayout {

	public:
		VulkanResourceLayout(std::initializer_list<Resource> resources);

		virtual void Destroy() override;
	private:
		VulkanResourceLayoutData m_VulkanData = {};
	};
}