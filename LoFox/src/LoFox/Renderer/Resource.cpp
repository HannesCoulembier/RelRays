#include "lfpch.h"
#include "LoFox/Renderer/Resource.h"

#include "LoFox/Renderer/RenderContext.h"
#include "LoFox/Renderer/Renderer.h"

namespace LoFox {

	Resource::Resource(VkShaderStageFlags shaderStage, Ref<LoFox::TextureAtlas> atlas)
		: Type(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER), ShaderStage(shaderStage), TexAtlas(atlas) {

		ImageDescriptorInfos = TexAtlas->GetDescriptorInfos();
		DescriptorCount = TexAtlas->GetTexCount();
	}

	Resource::Resource(VkShaderStageFlags shaderStage, Ref<LoFox::UniformBuffer> uniformBuffer)
		: Type(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER), ShaderStage(shaderStage), UniformBuffer(uniformBuffer) {

		BufferDescriptorInfos = UniformBuffer->GetDescriptorInfos();
	}

	Resource::Resource(VkShaderStageFlags shaderStage, Ref<LoFox::StorageBuffer> storageBuffer)
		: Type(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER), ShaderStage(shaderStage), StorageBuffer(storageBuffer) {

		BufferDescriptorInfos = StorageBuffer->GetDescriptorInfos();
	}

	ResourceLayout::ResourceLayout(std::initializer_list<Resource> resources)
		: m_Resources(resources) {

		std::vector<VkDescriptorSetLayoutBinding> bindings = {};
		uint32_t binding = 0;
		for (auto resource : resources) {

			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding.binding = binding;
			descriptorSetLayoutBinding.descriptorType = resource.Type;
			descriptorSetLayoutBinding.descriptorCount = resource.DescriptorCount;
			descriptorSetLayoutBinding.stageFlags = resource.ShaderStage;
			descriptorSetLayoutBinding.pImmutableSamplers = nullptr;

			bindings.push_back(descriptorSetLayoutBinding);
			binding++;
		}

		VkDescriptorSetLayoutCreateInfo descriptorSetlayoutCreateInfo = {};
		descriptorSetlayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptorSetlayoutCreateInfo.bindingCount = (uint32_t)bindings.size();
		descriptorSetlayoutCreateInfo.pBindings = bindings.data();

		LF_CORE_ASSERT(vkCreateDescriptorSetLayout(RenderContext::LogicalDevice, &descriptorSetlayoutCreateInfo, nullptr, &m_DescriptorSetLayout) == VK_SUCCESS, "Failed to create descriptor set layout!");
	}

	Ref<ResourceLayout> ResourceLayout::Create(std::initializer_list<Resource> resources) {
		return CreateRef<ResourceLayout>(resources);
	}

	void ResourceLayout::Destroy() {

		vkDestroyDescriptorSetLayout(RenderContext::LogicalDevice, m_DescriptorSetLayout, nullptr);
	}
}