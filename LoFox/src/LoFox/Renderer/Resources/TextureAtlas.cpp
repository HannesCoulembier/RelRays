#include "lfpch.h"
#include "LoFox/Renderer/Resources/TextureAtlas.h"

#include "LoFox/Renderer/Renderer.h"

namespace LoFox {

	TextureAtlas::TextureAtlas() {

	}

	Ref<TextureAtlas> TextureAtlas::Create() {

		return CreateRef<TextureAtlas>();
	}

	std::vector<VkDescriptorImageInfo> TextureAtlas::GetDescriptorInfos() {

		std::vector<VkDescriptorImageInfo> imageDescriptorInfos;

		for (size_t i = 0; i < m_Textures.size(); i++) {

			Ref<Texture> texture = m_Textures[i];
			VkDescriptorImageInfo imageDescriptorInfo = {};

			imageDescriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageDescriptorInfo.imageView = texture->GetImageView();
			imageDescriptorInfo.sampler = Renderer::GetImageSampler();

			imageDescriptorInfos.push_back(imageDescriptorInfo);
		}

		return imageDescriptorInfos;
	}

	void TextureAtlas::AddTexture(Ref<Texture> texture) {

		m_Textures.push_back(texture);
	}
}