#pragma once

#include "LoFox/Core/Core.h"

#include "LoFox/Renderer/Resources/Texture.h"

namespace LoFox {

	class TextureAtlas {

	public:
		TextureAtlas();

		void AddTexture(Ref<Texture> texture);

		std::vector<VkDescriptorImageInfo> GetDescriptorInfos();
		uint32_t GetTexCount() { return m_Textures.size(); }

		static Ref<TextureAtlas> Create();
	private:
		std::vector<Ref<Texture>> m_Textures;
	};
}