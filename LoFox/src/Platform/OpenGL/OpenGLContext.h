#pragma once
#include "LoFox/Renderer/GraphicsContext.h"

#include <glad/glad.h>

struct GLFWwindow;

namespace LoFox {

	class OpenGLContext : public GraphicsContext {

	public:
		static void Init(GLFWwindow* windowHandle);
		static void Shutdown();

		static void BeginFrame(glm::vec3 clearColor);
		static void SetActivePipeline(Ref<GraphicsPipeline> pipeline);
		static void Draw(Ref<IndexBuffer> indexBuffer, Ref<VertexBuffer> vertexBuffer);
		static void EndFrame();

		static void WaitIdle() {} // Does nothing as there is no multithreading

		static void PresentFrame();
	private:
		inline static GLFWwindow* m_WindowHandle;

		inline static GLuint m_VertexArrayID;
		inline static Ref<GraphicsPipeline> m_ActivePipeline;
	};

}