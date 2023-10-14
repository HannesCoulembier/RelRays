#pragma once

#include "LoFox/Renderer/Pipeline.h"

namespace LoFox {

	class GraphicsContext {

	public:
		static void Init(void* window);
		static void Shutdown();

		static void BeginFrame(glm::vec3 clearColor = { 0.0f, 0.0f, 0.0f });
		static void SetActivePipeline(Ref<GraphicsPipeline> pipeline);
		static void Draw(Ref<VertexBuffer> vertexBuffer);
		static void EndFrame();
		static void PresentFrame();

		static void WaitIdle();
	};
}