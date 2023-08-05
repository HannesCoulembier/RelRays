#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

#include "LoFox/Renderer/Shader.h"
#include "LoFox/Renderer/Resources/UniformBuffer.h"
#include "LoFox/Renderer/Resources/StorageBuffer.h"
#include "LoFox/Renderer/Resources/StorageImage.h"
#include "LoFox/Renderer/Resources/Texture.h"
#include "LoFox/Renderer/Resources/TextureAtlas.h"

namespace LoFox {

	struct Resource {

		VkDescriptorType Type;
		ShaderType ShaderStage;
		Ref<UniformBuffer> UniformBuffer = nullptr;
		Ref<StorageBuffer> StorageBuffer = nullptr;
		Ref<StorageImage> StorageImage = nullptr;
		Ref<TextureAtlas> TexAtlas = nullptr;
		uint32_t DescriptorCount = 1;

		std::vector<VkDescriptorBufferInfo> BufferDescriptorInfos;
		std::vector<VkDescriptorImageInfo> ImageDescriptorInfos;

		Resource(ShaderType shaderStage, Ref<LoFox::TextureAtlas> atlas);
		Resource(ShaderType shaderStage, Ref<LoFox::UniformBuffer> uniformBuffer);
		Resource(ShaderType shaderStage, Ref<LoFox::StorageBuffer> storageBuffer);
		Resource(ShaderType shaderStage, Ref<LoFox::StorageImage> storageImage, bool isDestination);
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