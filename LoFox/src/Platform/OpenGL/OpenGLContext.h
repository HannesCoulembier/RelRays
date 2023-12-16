#pragma once
#include "LoFox/Renderer/GraphicsContext.h"

#include <glad/glad.h>

#include "Platform/OpenGL/OpenGLDebugMessenger.h"

namespace LoFox {

	class OpenGLContext : public GraphicsContext {

	public:
		static void Init(Ref<Window> window);
		static void Shutdown();

		static void BeginFrame();
		static void BeginFramebuffer(Ref<Framebuffer> framebuffer, glm::vec3 clearColor);
		static void SetActivePipeline(Ref<GraphicsPipeline> pipeline);
		static void Draw(Ref<IndexBuffer> indexBuffer, Ref<VertexBuffer> vertexBuffer);
		static void EndFramebuffer();
		static void EndFrame();

		static void WaitIdle() {} // Does nothing as there is no multithreading

		static void PresentFrame();
	private:
		inline static Ref<Window> m_Window;

		inline static Ref<OpenGLDebugMessenger> m_DebugMessenger;

		inline static GLuint m_VertexArrayID;
		inline static Ref<GraphicsPipeline> m_ActivePipeline;

		// TODO: Give own FrameBuffer class
		inline static GLuint m_FramebufferID;
		inline static GLuint m_RenderbufferID;
		inline static GLuint m_FramebufferColorAttachmentID;
	};

}