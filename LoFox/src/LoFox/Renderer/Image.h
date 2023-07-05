#pragma once

#include "LoFox/Core/Core.h"

#include "LoFox/Renderer/RenderContext.h"
#include "LoFox/Renderer/Buffer.h"

namespace LoFox {

	class Image {

	public:
		Image(Ref<RenderContext> context, const std::string& path);
		~Image() { Destroy(); }

		void Destroy();

		inline int GetWidth() { return m_Width; }
		inline int GetHeight() { return m_Height; }
		inline int GetSize() { return m_Width * m_Height * 4; } // 4 floats per pixel
		inline VkImageView GetImageView() { return m_ImageView; }

		void TransitionLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	private:
		void CreateImageView();
	private:
		Ref<RenderContext> m_Context;
		const std::string& m_Path;

		VkImage m_Image;
		VkDeviceMemory m_Memory;
		VkImageView m_ImageView;

		int m_Width, m_Height;
	};
}