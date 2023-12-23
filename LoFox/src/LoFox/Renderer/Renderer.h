#pragma once

#include "LoFox/Core/Window.h"
#include "LoFox/Renderer/RendererAPI.h"

#include "LoFox/Renderer/Framebuffer.h"
#include "LoFox/Renderer/Pipeline.h"
#include "LoFox/Renderer/VertexBuffer.h"
#include "LoFox/Renderer/IndexBuffer.h"

namespace LoFox {

	class Renderer {

	public:
		static void Init(Ref<Window> window);
		static void Shutdown();

		static void BeginFrame();
		static void BeginFramebuffer(Ref<Framebuffer> framebuffer, glm::vec3 clearColor = { 0.0f, 0.0f, 0.0f });

		static void SetActivePipeline(Ref<GraphicsPipeline> pipeline);
		static void Draw(Ref<IndexBuffer> indexBuffer, Ref<VertexBuffer> vertexBuffer);

		static void EndFramebuffer();
		static void EndFrame();

		static void WaitIdle();

		static void OnResize(uint32_t width, uint32_t height) {}

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		inline static Ref<Window> m_Window;
	};
}