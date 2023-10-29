#pragma once

#include "LoFox/Renderer/Texture.h"

namespace LoFox {

	class VulkanTexture : public Texture {

	public:
		virtual void Destroy() override;
	};
}