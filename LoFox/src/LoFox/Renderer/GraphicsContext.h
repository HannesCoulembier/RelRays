#pragma once

#include "LoFox/Renderer/Framebuffer.h"
#include "LoFox/Renderer/Pipeline.h"
#include "LoFox/Renderer/VertexBuffer.h"
#include "LoFox/Renderer/IndexBuffer.h"
#include "LoFox/Core/Window.h"

namespace LoFox {

	class GraphicsContext {

	public:
		static void Init(Ref<Window> window);
		static void Shutdown();

		static void BeginFrame();
		static void BeginFramebuffer(Ref<Framebuffer> framebuffer, glm::vec3 clearColor = { 0.0f, 0.0f, 0.0f });
		static void SetActivePipeline(Ref<GraphicsPipeline> pipeline);
		static void Draw(Ref<IndexBuffer> indexBuffer, Ref<VertexBuffer> vertexBuffer);
		static void EndFramebuffer();
		static void EndFrame();
		static void PresentFrame();

		static void WaitIdle();
	};
}