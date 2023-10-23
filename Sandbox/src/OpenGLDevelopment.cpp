#include "OpenGLDevelopment.h"

namespace LoFox {

	void OpenGLDevLayer::OnAttach() {
		
		m_FragmentShader = Shader::Create(ShaderType::Fragment, "Assets/Shaders/OpenGLDevelopment/FragmentShader.frag");
		m_VertexShader = Shader::Create(ShaderType::Vertex, "Assets/Shaders/OpenGLDevelopment/VertexShader.vert");

		VertexLayout layout = { // Must match QuadVertex
			{ VertexAttributeType::Float3 }, // position
			{ VertexAttributeType::Float3 }, // color
		};

		uint32_t vertexBufferSize = sizeof(vertices[0]) * vertices.size();
		m_VertexBuffer = VertexBuffer::Create(vertexBufferSize, vertices.data(), layout);

		GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
		graphicsPipelineCreateInfo.VertexShader = m_VertexShader;
		graphicsPipelineCreateInfo.FragmentShader = m_FragmentShader;
		graphicsPipelineCreateInfo.VertexLayout = layout;
		graphicsPipelineCreateInfo.Topology = Topology::Triangle;
		graphicsPipelineCreateInfo.LineWidth = 10.0f;
		graphicsPipelineCreateInfo.PointSize = 100.0f;

		m_Pipeline = GraphicsPipeline::Create(graphicsPipelineCreateInfo);
	}
	void OpenGLDevLayer::OnDetach() {

		m_VertexBuffer->Destroy();
		m_VertexShader->Destroy();
		m_FragmentShader->Destroy();
		m_Pipeline->Destroy();
	}

	void OpenGLDevLayer::OnUpdate(float ts) {

		m_Time += ts;
		static float avgFPS = 0;
		static uint32_t frames = 0;
		frames++;
		float FPS = 1.0f / ts;
		avgFPS += (FPS - avgFPS) / (float)frames;
		Application::GetInstance().GetActiveWindow()->SetTitle("Sandbox Application: " + std::to_string(FPS) + " FPS (avg: " + std::to_string(avgFPS) + ")" + " Time: " + std::to_string(m_Time));

		Renderer::BeginFrame({ glm::abs(glm::sin(2.0f*m_Time)), 1.0f, 0.0f });

		Renderer::SetActivePipeline(m_Pipeline);
		Renderer::Draw(m_VertexBuffer);

		Renderer::EndFrame();

	}
	void OpenGLDevLayer::OnEvent(LoFox::Event& event) {

	}
}