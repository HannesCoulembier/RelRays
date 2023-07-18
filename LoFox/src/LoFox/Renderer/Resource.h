#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

#include "LoFox/Renderer/Buffer.h"
#include "LoFox/Renderer/Image.h"

namespace LoFox {

	struct Resource {

		VkDescriptorType Type;
		VkShaderStageFlags ShaderStage;
		std::vector<Ref<Buffer>> Buffers;
		Ref<Image> Texture;

		std::vector<VkDescriptorBufferInfo> BufferDescriptorInfos;
		VkDescriptorImageInfo ImageDescriptorInfo;

		Resource(VkDescriptorType type, VkShaderStageFlags shaderStage, std::vector<Ref<Buffer>> buffers, Ref<Image> image);
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