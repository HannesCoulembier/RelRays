#pragma once

#include "LoFox/Core/Core.h"

#include "LoFox/Renderer/Shader.h"
#include "LoFox/Renderer/Resources/UniformBuffer.h"
#include "LoFox/Renderer/Resources/StorageBuffer.h"
// #include "LoFox/Renderer/Resources/StorageImage.h"
// #include "LoFox/Renderer/Resources/Texture.h"
// #include "LoFox/Renderer/Resources/TextureAtlas.h"

namespace LoFox {

	enum ResourceType {

		UniformBufferResource,
		StorageBufferResource,
	};

	struct Resource {

		ResourceType Type;
		ShaderType ShaderStage;
		bool IsBuffer = false;
		bool IsImage = false;
		Ref<UniformBuffer> UniformBufferRef = nullptr;
		Ref<StorageBuffer> StorageBufferRef = nullptr;
		// Ref<StorageImage> StorageImageRef = nullptr;
		// Ref<TextureAtlas> TexAtlasRef = nullptr;
		uint32_t ItemCount = 1; // TexAtlas will have multiple textures -> multiple items

		// std::vector<VkDescriptorBufferInfo> BufferDescriptorInfos;
		// std::vector<VkDescriptorImageInfo> ImageDescriptorInfos;

		// Resource(ShaderType shaderStage, Ref<LoFox::TextureAtlas> atlas);
		Resource(ShaderType shaderStage, Ref<UniformBuffer> uniformBuffer);
		Resource(ShaderType shaderStage, Ref<StorageBuffer> storageBuffer);
		// Resource(ShaderType shaderStage, Ref<LoFox::StorageImage> storageImage, bool isDestination);
	};

	class ResourceLayout {

	public:
		virtual void Destroy() = 0;

		// VkDescriptorSetLayout GetDescriptorSetLayout() { return m_DescriptorSetLayout; }
		std::vector<Resource> GetResources() { return m_Resources; }
		void* GetData() { return m_Data; }

		static Ref<ResourceLayout> Create(std::initializer_list<Resource> resources);
	protected:
		ResourceLayout(std::initializer_list<Resource> resources);
	protected:
		void* m_Data = nullptr;
		std::vector<Resource> m_Resources;

		// VkDescriptorSetLayout m_DescriptorSetLayout;
	};
}