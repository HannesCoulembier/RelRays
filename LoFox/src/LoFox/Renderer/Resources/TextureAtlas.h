#pragma once

#include "LoFox/Core/Core.h"

#include "LoFox/Renderer/Resources/Texture.h"

namespace LoFox {

	class TextureAtlas {

	public:
		virtual void Destroy() = 0;

		// std::vector<VkDescriptorImageInfo> GetDescriptorInfos();
		void* GetData() { return m_Data; }
		uint32_t GetTextureCount() { return m_Textures.size(); }

		static Ref<TextureAtlas> Create(uint32_t space, std::vector<Ref<Texture>> textures);
	protected:
		TextureAtlas(uint32_t space, std::vector<Ref<Texture>> textures);
	protected:
		void* m_Data = nullptr;

		uint32_t m_Space;
		std::vector<Ref<Texture>> m_Textures;
	};
}