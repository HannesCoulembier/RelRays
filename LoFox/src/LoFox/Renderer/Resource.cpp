#include "lfpch.h"
#include "LoFox/Renderer/Resource.h"

#include "Platform/Vulkan/VulkanContext.h"
#include "LoFox/Renderer/Renderer.h"

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/OpenGLResource.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/VulkanResource.h"
#endif

namespace LoFox {

	// Resource::Resource(ShaderType shaderStage, Ref<LoFox::TextureAtlas> atlas)
	// 	: Type(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER), ShaderStage(shaderStage), TexAtlas(atlas) {
	// 
	// 	ImageDescriptorInfos = TexAtlas->GetDescriptorInfos();
	// 	DescriptorCount = TexAtlas->GetTexCount();
	// }

	Resource::Resource(ShaderType shaderStage, Ref<LoFox::UniformBuffer> uniformBuffer)
		: Type(ResourceType::UniformBufferResource), ShaderStage(shaderStage), IsBuffer(true), UniformBufferRef(uniformBuffer) {

		// BufferDescriptorInfos = UniformBufferRef->GetDescriptorInfos();
	}

	// Resource::Resource(ShaderType shaderStage, Ref<LoFox::StorageBuffer> storageBuffer)
	// 	: Type(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER), ShaderStage(shaderStage), StorageBuffer(storageBuffer) {
	// 
	// 	BufferDescriptorInfos = StorageBuffer->GetDescriptorInfos();
	// }

	// Resource::Resource(ShaderType shaderStage, Ref<LoFox::StorageImage> storageImage, bool isDestination)
	// 	: ShaderStage(shaderStage), StorageImage(storageImage) {
	// 
	// 	if (isDestination)
	// 		Type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	// 	else
	// 		Type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	// 
	// 	ImageDescriptorInfos.clear();
	// 	ImageDescriptorInfos.push_back(StorageImage->GetDescriptorInfo());
	// }

	ResourceLayout::ResourceLayout(std::initializer_list<Resource> resources)
		: m_Resources(resources) {

		// std::vector<VkDescriptorSetLayoutBinding> bindings = {};
		// uint32_t binding = 0;
		// for (auto resource : resources) {
		// 
		// 	VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
		// 	descriptorSetLayoutBinding.binding = binding;
		// 	descriptorSetLayoutBinding.descriptorType = resource.Type;
		// 	descriptorSetLayoutBinding.descriptorCount = resource.DescriptorCount;
		// 	descriptorSetLayoutBinding.stageFlags = ShaderTypeToVulkan(resource.ShaderStage);
		// 	descriptorSetLayoutBinding.pImmutableSamplers = nullptr;
		// 
		// 	bindings.push_back(descriptorSetLayoutBinding);
		// 	binding++;
		// }
		// 
		// VkDescriptorSetLayoutCreateInfo descriptorSetlayoutCreateInfo = {};
		// descriptorSetlayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		// descriptorSetlayoutCreateInfo.bindingCount = (uint32_t)bindings.size();
		// descriptorSetlayoutCreateInfo.pBindings = bindings.data();
		// 
		// LF_CORE_ASSERT(vkCreateDescriptorSetLayout(VulkanContext::LogicalDevice, &descriptorSetlayoutCreateInfo, nullptr, &m_DescriptorSetLayout) == VK_SUCCESS, "Failed to create descriptor set layout!");
	}

	Ref<ResourceLayout> ResourceLayout::Create(std::initializer_list<Resource> resources) {

		#ifdef LF_RENDERAPI_OPENGL
			return CreateRef<OpenGLResourceLayout>(resources);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			return CreateRef<VulkanResourceLayout>(resources);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	// void ResourceLayout::Destroy() {
	// 
	// 	// vkDestroyDescriptorSetLayout(VulkanContext::LogicalDevice, m_DescriptorSetLayout, nullptr);
	// }
}