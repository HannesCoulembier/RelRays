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
	// 	: Type(ResourceType::TextureAtlasResource),		ShaderStage(shaderStage), IsImage(true), TextureAtlasRef(atlas) {
	// 
	// 	ItemCount = TextureAtlasRef->GetTextureCount();
	// }

	Resource::Resource(ShaderType shaderStage, Ref<LoFox::UniformBuffer> uniformBuffer)
		: Type(ResourceType::UniformBufferResource),	ShaderStage(shaderStage), IsBuffer(true), UniformBufferRef(uniformBuffer) {}

	Resource::Resource(ShaderType shaderStage, Ref<LoFox::StorageBuffer> storageBuffer)
		: Type(ResourceType::StorageBufferResource),	ShaderStage(shaderStage), IsBuffer(true), StorageBufferRef(storageBuffer) {}

	Resource::Resource(ShaderType shaderStage, Ref<LoFox::Texture> texture)
		: Type(ResourceType::TextureResource),			ShaderStage(shaderStage), IsImage(true), TextureRef(texture) {}

	Resource::Resource(ShaderType shaderStage, Ref<LoFox::StorageImage> image, bool interpretAsTexture)
		:												ShaderStage(shaderStage), IsImage(true), StorageImageRef(image) {
		if (interpretAsTexture) Type = ResourceType::StorageImageAsTextureResource; // usefull for raytracing
		else Type = ResourceType::StorageImageResource;
	}

	ResourceLayout::ResourceLayout(std::initializer_list<Resource> resources)
		: m_Resources(resources) {}

	Ref<ResourceLayout> ResourceLayout::Create(std::initializer_list<Resource> resources) {

		#ifdef LF_RENDERAPI_OPENGL
			return CreateRef<OpenGLResourceLayout>(resources);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			return CreateRef<VulkanResourceLayout>(resources);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}
}