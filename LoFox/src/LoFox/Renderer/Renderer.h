#pragma once

// #include <vulkan/vulkan.h>

#include "LoFox/Core/Window.h"
#include "LoFox/Renderer/RendererAPI.h"
// #include "LoFox/Renderer/RenderCommand.h"

// #include "LoFox/Renderer/Buffer.h"
// #include "LoFox/Renderer/Image.h"

// #include "LoFox/Renderer/Pipeline.h"
// #include "LoFox/Renderer/SwapChain.h"

#include "LoFox/Renderer/Pipeline.h"
#include "LoFox/Renderer/VertexBuffer.h"
#include "LoFox/Renderer/IndexBuffer.h"
// #include "LoFox/Events/RenderEvent.h"

#include "LoFox/Utils/Time.h"

namespace LoFox {

	class Renderer {

	public:
		static void Init(Ref<Window> window);
		static void Shutdown();

		static void BeginFrame(glm::vec3 clearColor = { 0.0f, 0.0f, 0.0f });

		static void SetActivePipeline(Ref<GraphicsPipeline> pipeline);
		static void Draw(Ref<IndexBuffer> indexBuffer, Ref<VertexBuffer> vertexBuffer);

		static void EndFrame();

		static void WaitIdle();

		// static void SetResourceLayout(Ref<ResourceLayout> layout);
		// static void SubmitGraphicsPipeline(Ref<GraphicsPipeline> pipeline);
		// 
		// static void PrepareFrame();
		// static void StartFrame();
		// static void Draw(uint32_t instances);
		// static void SubmitFrame();
		// 
		// static void WaitIdle();
		// 
		// static inline VkCommandBuffer GetCommandBuffer() { return m_SwapChain->GetThisFramesCommandbuffer(); }
		// static inline VkFormat GetSwapChainImageFormat() { return m_SwapChain->GetImageFormat(); }
		// static inline VkFormat GetSwapChainDepthFormat() { return m_SwapChain->GetDepthImage()->GetFormat(); }
		// static inline VkExtent2D GetSwapChainExtent() { return m_SwapChain->GetExtent(); }
		// static inline uint32_t GetCurrentFrame() { return m_SwapChain->GetCurrentFrame(); }
		// static inline VkSampler GetImageSampler() { return m_Sampler; }

		static void OnResize(uint32_t width, uint32_t height) {
			// OnFramebufferResize(FramebufferResizeEvent(width, height));
		}

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	public:
		// static const int MaxFramesInFlight = 1;
	private:
		inline static Ref<Window> m_Window;
		// static Ref<SwapChain> m_SwapChain;

		inline static Timer m_Timer;

		// inline static Ref<GraphicsPipeline> m_GraphicsPipeline;
		// inline static Ref<ResourceLayout> m_ResourceLayout;
		// 
		// inline static VkSampler m_Sampler;
		// 
		// inline static VkDescriptorPool m_DescriptorPool;
		// inline static std::vector<VkDescriptorSet> m_GraphicsDescriptorSets;
	private:
		// static void CreateDescriptorPool();

		// static bool OnFramebufferResize(FramebufferResizeEvent& event);
	};
}