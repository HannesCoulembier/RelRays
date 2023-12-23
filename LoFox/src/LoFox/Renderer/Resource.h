#pragma once

#include "LoFox/Renderer/Shader.h"
#include "LoFox/Renderer/Resources/UniformBuffer.h"
#include "LoFox/Renderer/Resources/StorageBuffer.h"
#include "LoFox/Renderer/Resources/StorageImage.h"
#include "LoFox/Renderer/Resources/Texture.h"
// #include "LoFox/Renderer/Resources/TextureAtlas.h"

namespace LoFox {

	enum ResourceType {

		UniformBufferResource,
		StorageBufferResource,
		TextureResource,
		StorageImageResource,
		StorageImageAsTextureResource,
		// TextureAtlasResource,
	};

	struct Resource {

		ResourceType Type;
		ShaderType ShaderStage;
		bool IsBuffer = false;
		bool IsImage = false;
		Ref<UniformBuffer> UniformBufferRef = nullptr;
		Ref<StorageBuffer> StorageBufferRef = nullptr;
		Ref<Texture> TextureRef = nullptr;
		Ref<StorageImage> StorageImageRef = nullptr;
		// Ref<TextureAtlas> TextureAtlasRef = nullptr;
		uint32_t ItemCount = 1; // TexAtlas will have multiple textures -> multiple items

		// Resource(ShaderType shaderStage, Ref<TextureAtlas> atlas);
		Resource(ShaderType shaderStage, Ref<UniformBuffer> uniformBuffer);
		Resource(ShaderType shaderStage, Ref<StorageBuffer> storageBuffer);
		Resource(ShaderType shaderStage, Ref<Texture> texture);
		Resource(ShaderType shaderStage, Ref<StorageImage> storageImage, bool interpretAsTexture = false);
	};

	class ResourceLayout {

	public:
		virtual void Destroy() = 0;

		std::vector<Resource> GetResources() { return m_Resources; }
		void* GetData() { return m_Data; }

		static Ref<ResourceLayout> Create(std::initializer_list<Resource> resources);
	protected:
		ResourceLayout(std::initializer_list<Resource> resources);
	protected:
		void* m_Data = nullptr;
		std::vector<Resource> m_Resources;
	};
}