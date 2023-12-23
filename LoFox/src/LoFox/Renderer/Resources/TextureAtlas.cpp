#include "lfpch.h"

// TODO: add back but good
/*
#include "LoFox/Renderer/Resources/TextureAtlas.h"

#include "LoFox/Renderer/Renderer.h"

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/Resources/OpenGLTextureAtlas.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/Resources/VulkanTextureAtlas.h"
#endif

namespace LoFox {

	TextureAtlas::TextureAtlas(uint32_t space, std::vector<Ref<Texture>> textures)
		: m_Space(space) {

		m_Textures = textures;
		m_Textures.reserve(m_Space);
	}

	Ref<TextureAtlas> TextureAtlas::Create(uint32_t space, std::vector<Ref<Texture>> textures) {
		
		#ifdef LF_RENDERAPI_OPENGL
				return CreateRef<OpenGLTextureAtlas>(space, textures);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
				return CreateRef<VulkanTextureAtlas>(space, textures);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	// std::vector<VkDescriptorImageInfo> TextureAtlas::GetDescriptorInfos() {
	// 
	// 	std::vector<VkDescriptorImageInfo> imageDescriptorInfos;
	// 
	// 	for (size_t i = 0; i < m_Textures.size(); i++) {
	// 
	// 		Ref<Texture> texture = m_Textures[i];
	// 		VkDescriptorImageInfo imageDescriptorInfo = {};
	// 
	// 		imageDescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	// 		imageDescriptorInfo.imageView = texture->GetImageView();
	// 		imageDescriptorInfo.sampler = Renderer::GetImageSampler();
	// 
	// 		imageDescriptorInfos.push_back(imageDescriptorInfo);
	// 	}
	// 
	// 	return imageDescriptorInfos;
	// }
}
*/