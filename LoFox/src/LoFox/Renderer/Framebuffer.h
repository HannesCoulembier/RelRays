#pragma once

namespace LoFox {

	enum class FramebufferTextureFormat {

		None = 0,

		// Color
		RGBA8,
	};

	struct FramebufferTextureDescription {

		FramebufferTextureDescription() = default;
		FramebufferTextureDescription(FramebufferTextureFormat format)
			: TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
	};

	struct FramebufferCreateInfo {

		uint32_t Width = 0, Height = 0;
		std::vector<FramebufferTextureDescription> Attachments;

		bool SwapChainTarget = false; // When true, this means one should not use the framebuffer as an image
	};

	class Framebuffer {

	public:
		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void Destroy() = 0;

		virtual void* GetAttachmentImTextureID(uint32_t index) = 0;

		bool IsSwapChainTarget() { return m_CreateInfo.SwapChainTarget; }
		void* GetData() { return m_Data; }
		uint32_t GetWidth() { return m_CreateInfo.Width; }
		uint32_t GetHeight() { return m_CreateInfo.Height; }

		static Ref<Framebuffer> Create(FramebufferCreateInfo createInfo);
	protected:
		Framebuffer(FramebufferCreateInfo createInfo);
		Framebuffer() {}
	protected:
		FramebufferCreateInfo m_CreateInfo;
		void* m_Data = nullptr;
	};
}