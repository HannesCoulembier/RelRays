#include "lfpch.h"
#include "LoFox/Renderer/Renderer.h"

#include "LoFox/Core/Application.h"

#include "LoFox/Renderer/GraphicsContext.h"

struct UniformBufferObject {

	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

namespace LoFox {

	void Renderer::Init(Ref<Window> window) {

		m_Window = window;
	}

	void Renderer::BeginFrame() {

		GraphicsContext::BeginFrame();
	}

	void Renderer::BeginFramebuffer(Ref<Framebuffer> framebuffer, glm::vec3 clearColor) {

		GraphicsContext::BeginFramebuffer(framebuffer, clearColor);
	}

	void Renderer::SetActivePipeline(Ref<GraphicsPipeline> pipeline) {

		GraphicsContext::SetActivePipeline(pipeline);
	}

	void Renderer::Draw(Ref<IndexBuffer> indexBuffer, Ref<VertexBuffer> vertexBuffer) {

		GraphicsContext::Draw(indexBuffer, vertexBuffer);
	}

	void Renderer::EndFramebuffer() {

		GraphicsContext::EndFramebuffer();
	}

	void Renderer::EndFrame() {

		GraphicsContext::EndFrame();
	}

	void Renderer::WaitIdle() {

		GraphicsContext::WaitIdle();
	}
	
	void Renderer::Shutdown() {}
}