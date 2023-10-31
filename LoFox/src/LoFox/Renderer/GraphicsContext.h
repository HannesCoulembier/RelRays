#pragma once

#include "LoFox/Renderer/Pipeline.h"
#include "LoFox/Renderer/VertexBuffer.h"
#include "LoFox/Renderer/IndexBuffer.h"

namespace LoFox {

	class GraphicsContext {

	public:
		static void Init(void* window);
		static void Shutdown();

		static void BeginFrame(glm::vec3 clearColor = { 0.0f, 0.0f, 0.0f });
		static void SetActivePipeline(Ref<GraphicsPipeline> pipeline);
		static void Draw(Ref<IndexBuffer> indexBuffer, Ref<VertexBuffer> vertexBuffer);
		static void EndFrame();
		static void PresentFrame();

		static void WaitIdle();
	};
}