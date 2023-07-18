#include "lfpch.h"
#include "LoFox/Renderer/Resource.h"

#include "LoFox/Renderer/RenderContext.h"
#include "LoFox/Renderer/Renderer.h"

namespace LoFox {

	Resource::Resource(VkDescriptorType type, VkShaderStageFlags shaderStage, std::vector<Ref<Buffer>> buffers, Ref<Image> texture)
		: Type(type), ShaderStage(shaderStage), Buffers(buffers), Texture(texture) {
		
		for (const auto& buffer : Buffers) {

			VkDescriptorBufferInfo bufferDescriptorInfo = {};
			bufferDescriptorInfo.buffer = VK_NULL_HANDLE;
			if (Type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {

				bufferDescriptorInfo.buffer = buffer->GetBuffer();
				bufferDescriptorInfo.offset = 0;
				bufferDescriptorInfo.range = 128; // TODO: Fix this hardcoding by making proper UniformBuffer class to replace std::vector<Ref<Buffer>> (this should hold UniformBufferObject)
				// bufferDescriptorInfo.range = sizeof(UniformBufferObject);
			}

			BufferDescriptorInfos.push_back(bufferDescriptorInfo);
		}

		ImageDescriptorInfo = {};
		ImageDescriptorInfo.imageView = VK_NULL_HANDLE;
		if (Type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {

			ImageDescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			ImageDescriptorInfo.imageView = Texture->GetImageView();
			ImageDescriptorInfo.sampler = Renderer::GetImageSampler();
		}
	}

	ResourceLayout::ResourceLayout(std::initializer_list<Resource> resources)
		: m_Resources(resources) {

		std::vector<VkDescriptorSetLayoutBinding> bindings = {};
		uint32_t binding = 0;
		for (auto resource : resources) {

			VkDescriptorSetLayoutBinding descriptorSetLayoutBinding = {};
			descriptorSetLayoutBinding.binding = binding;
			descriptorSetLayoutBinding.descriptorType = resource.Type;
			descriptorSetLayoutBinding.descriptorCount = 1;
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