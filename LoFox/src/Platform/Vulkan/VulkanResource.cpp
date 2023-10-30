#include "lfpch.h"
#include "Platform/Vulkan/VulkanResource.h"

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanShader.h"

#include "Platform/Vulkan/Resources/VulkanUniformBuffer.h"
#include "Platform/Vulkan/Resources/VulkanStorageBuffer.h"
#include "Platform/Vulkan/Resources/VulkanTexture.h"

namespace LoFox {

	VkDescriptorType ResourceTypeToVulkanDescriptorType(ResourceType type) {

		switch (type) {
			case ResourceType::UniformBufferResource:	return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case ResourceType::StorageBufferResource:	return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case ResourceType::TextureResource:			return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		}

		LF_CORE_ASSERT(false, "Unknown ResourceType");
	}

	VkDescriptorBufferInfo GetVkDescriptorBufferInfoFromResource(Resource resource) {

		if (resource.UniformBufferRef) {
			VulkanUniformBufferData* uniformBufferData = static_cast<VulkanUniformBufferData*>(resource.UniformBufferRef->GetData());
			return uniformBufferData->BufferInfo;
		}

		if (resource.StorageBufferRef) {
			VulkanStorageBufferData* storageBufferData = static_cast<VulkanStorageBufferData*>(resource.StorageBufferRef->GetData());
			return storageBufferData->BufferInfo;
		}

		LF_CORE_ASSERT(false, "Resource is not a buffer!");
	}

	VkDescriptorImageInfo GetVkDescriptorImageInfoFromResource(Resource resource) {

		if (resource.TextureRef) {
			VulkanTextureData* textureData = static_cast<VulkanTextureData*>(resource.TextureRef->GetData());
			return textureData->ImageInfo;
		}

		LF_CORE_ASSERT(false, "Resource is not an image!");
		VkDescriptorImageInfo error = {};
		return error;
	}

	VulkanResourceLayout::VulkanResourceLayout(std::initializer_list<Resource> resources)
		: ResourceLayout(resources) {

		m_Data = &m_VulkanData;

		std::vector<VkDescriptorSetLayoutBinding> bindings = {};
		uint32_t binding = 0;
		for (auto resource : resources) {
		
			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding.binding = binding;
			descriptorSetLayoutBinding.descriptorType = ResourceTypeToVulkanDescriptorType(resource.Type);
			descriptorSetLayoutBinding.descriptorCount = resource.ItemCount;
			descriptorSetLayoutBinding.stageFlags = ShaderTypeToVulkanShaderStage(resource.ShaderStage);
			descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
		
			bindings.push_back(descriptorSetLayoutBinding);
			binding++;
		}
		
		VkDescriptorSetLayoutCreateInfo descriptorSetlayoutCreateInfo = {};
		descriptorSetlayoutCreateInfo.bindingCount = (uint32_t)bindings.size();
		descriptorSetlayoutCreateInfo.flags = 0;
		descriptorSetlayoutCreateInfo.pBindings = bindings.data();
		descriptorSetlayoutCreateInfo.pNext = nullptr;
		descriptorSetlayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		
		LF_CORE_ASSERT(vkCreateDescriptorSetLayout(VulkanContext::LogicalDevice, &descriptorSetlayoutCreateInfo, nullptr, &m_VulkanData.DescriptorSetLayout) == VK_SUCCESS, "Failed to create descriptor set layout!");
	}

	void VulkanResourceLayout::Destroy() {

		vkDestroyDescriptorSetLayout(VulkanContext::LogicalDevice, m_VulkanData.DescriptorSetLayout, nullptr);
	}
}