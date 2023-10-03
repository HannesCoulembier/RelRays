#pragma once

namespace LoFox {

	class GraphicsContext {

	public:
		static void Init(void* window);
		static void Shutdown();

		static void BeginFrame(glm::vec3 clearColor = { 0.0f, 0.0f, 0.0f });
		static void EndFrame();
		static void PresentFrame();
	};
}