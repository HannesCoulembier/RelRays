#pragma once

#include <vulkan/vulkan.h>

#include "LoFox/Core/Window.h"

#include "LoFox/Renderer/Buffer.h"
#include "LoFox/Renderer/Image.h"

#include "LoFox/Events/RenderEvent.h"

#include "LoFox/Utils/Time.h"

namespace LoFox {

	class Renderer {

	public:
		static void Init(Ref<Window> window);
		static void Shutdown();

		static void StartFrame();
		static void SubmitFrame();

		static void WaitIdle();

		static void OnResize(uint32_t width, uint32_t height) { OnFramebufferResize(FramebufferResizeEvent(width, height)); }
	private:
		static void RecordCommandBuffer(VkCommandBuffer commandBuffer);

		static void UpdateUniformBuffer(uint32_t currentImage);
		static void CreateImageSampler();

		static bool OnFramebufferResize(FramebufferResizeEvent& event);
	private:
		static Ref<Window> m_Window;
		static Timer m_Timer;

		static Ref<Image> m_Texture1;

		static std::vector<Ref<Buffer>> m_UniformBuffers;
		static std::vector<void*> m_UniformBuffersMapped;

		static VkSampler m_Sampler;

		static Ref<Buffer> m_VertexBuffer;
		static Ref<Buffer> m_IndexBuffer;

		static VkDescriptorPool m_DescriptorPool;
		static VkDescriptorSetLayout m_DescriptorSetLayout;
		static std::vector<VkDescriptorSet> m_DescriptorSets;

		static uint32_t m_ThisFramesImageIndex;

		static int m_CurrentFrame;
		static const int m_MaxFramesInFlight = 1;
	};
}