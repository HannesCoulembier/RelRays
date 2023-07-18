#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

#include "LoFox/Renderer/Resources/UniformBuffer.h"
#include "LoFox/Renderer/Resources/Texture.h"

namespace LoFox {

	struct Resource {

		VkDescriptorType Type;
		VkShaderStageFlags ShaderStage;
		Ref<UniformBuffer> Buffer;
		Ref<Texture> Texture;

		std::vector<VkDescriptorBufferInfo> BufferDescriptorInfos;
		VkDescriptorImageInfo ImageDescriptorInfo;

		Resource(VkDescriptorType type, VkShaderStageFlags shaderStage, Ref<UniformBuffer> buffer, Ref<LoFox::Texture> texture);
	};

	class ResourceLayout {

	public:
		ResourceLayout(std::initializer_list<Resource> resources);
		void Destroy();

		VkDescriptorSetLayout GetDescriptorSetLayout() { return m_DescriptorSetLayout; }
		std::vector<Resource> GetResources() { return m_Resources; }

		static Ref<ResourceLayout> Create(std::initializer_list<Resource> resources);
	private:
		std::vector<Resource> m_Resources;

		VkDescriptorSetLayout m_DescriptorSetLayout;
	};
}