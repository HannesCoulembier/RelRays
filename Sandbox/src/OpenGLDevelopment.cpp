#include "OpenGLDevelopment.h"

namespace LoFox {

	void OpenGLDevLayer::OnAttach() {

		m_CameraData = UniformBuffer::Create(sizeof(UBO));
		m_ObjectTransforms = StorageBuffer::Create(1000, sizeof(glm::mat4));
		m_RickTexture = Texture::Create("Assets/Textures/Rick.png");
		m_PolandTexture = Texture::Create("Assets/Textures/poland.png");

		m_ResourceLayout = ResourceLayout::Create({
			{ ShaderType::Vertex, m_CameraData },
			{ ShaderType::Vertex, m_ObjectTransforms },
			{ ShaderType::Fragment, m_RickTexture },
			{ ShaderType::Fragment, m_PolandTexture },
		});
		
		m_FragmentShader = Shader::Create(ShaderType::Fragment, "Assets/Shaders/OpenGLDevelopment/FragmentShader.frag");
		m_VertexShader = Shader::Create(ShaderType::Vertex, "Assets/Shaders/OpenGLDevelopment/VertexShader.vert");

		VertexLayout layout = { // Must match QuadVertex
			{ VertexAttributeType::Float3 }, // position
			{ VertexAttributeType::Float3 }, // color
			{ VertexAttributeType::Float2 }, // uv
		};

		uint32_t vertexBufferSize = sizeof(vertices[0]) * vertices.size();
		m_VertexBuffer = VertexBuffer::Create(vertexBufferSize, vertices.data(), layout);
		m_IndexBuffer = IndexBuffer::Create(vertexIndices.size(), vertexIndices.data());

		GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
		graphicsPipelineCreateInfo.VertexShader = m_VertexShader;
		graphicsPipelineCreateInfo.FragmentShader = m_FragmentShader;
		graphicsPipelineCreateInfo.ResourceLayout = m_ResourceLayout;
		graphicsPipelineCreateInfo.VertexLayout = layout;
		graphicsPipelineCreateInfo.Topology = Topology::Triangle;
		graphicsPipelineCreateInfo.LineWidth = 10.0f;
		graphicsPipelineCreateInfo.PointSize = 100.0f;

		m_Pipeline = GraphicsPipeline::Create(graphicsPipelineCreateInfo);
	}
	void OpenGLDevLayer::OnDetach() {

		m_ResourceLayout->Destroy();
		m_CameraData->Destroy();
		m_ObjectTransforms->Destroy();
		m_RickTexture->Destroy();
		m_PolandTexture->Destroy();

		m_IndexBuffer->Destroy();
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

		// TODO: move to function
		UBO ubo = {};
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
		glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f); // Forward into the screen goes into the negative z-direction.
		ubo.view = glm::lookAt(cameraPos, cameraPos + forward, glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.proj = glm::perspectiveFov(glm::radians(45.0f), (float)Application::GetInstance().GetActiveWindow()->GetWindowData().Width, (float)Application::GetInstance().GetActiveWindow()->GetWindowData().Width, 0.1f, 4000.0f);
		
		ubo.invView = glm::inverse(ubo.view);
		ubo.invProj = glm::inverse(ubo.proj);

		m_CameraData->SetData(&ubo);

		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(m_Time, 0.0f, 0.0f));
		std::vector<glm::mat4> translationMatrices = { translationMatrix, translationMatrix };
		m_ObjectTransforms->SetData(1, translationMatrices.data());
		// END TODO

		Renderer::BeginFrame({ glm::abs(glm::sin(2.0f*m_Time)), 1.0f, 0.0f });

		Renderer::SetActivePipeline(m_Pipeline);
		Renderer::Draw(m_IndexBuffer, m_VertexBuffer);

		Renderer::EndFrame();

	}
	void OpenGLDevLayer::OnEvent(LoFox::Event& event) {

	}
}