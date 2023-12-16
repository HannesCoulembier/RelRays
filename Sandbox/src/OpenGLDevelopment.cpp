#include "OpenGLDevelopment.h"

namespace LoFox {

	void OpenGLDevLayer::OnAttach() {

		m_CameraData = UniformBuffer::Create(sizeof(UBO));
		m_ObjectTransforms = StorageBuffer::Create(1000, sizeof(glm::mat4));

		m_RickTexture = Texture::Create("Assets/Textures/Rick.png");
		m_PolandTexture = Texture::Create("Assets/Textures/poland.png");
		// m_TextureAtlas = TextureAtlas::Create(32, { m_RickTexture, m_PolandTexture });
		m_TestStorageImage = StorageImage::Create(m_RickTexture->GetWidth(), m_RickTexture->GetHeight());

		m_ResourceLayout = ResourceLayout::Create({
			{ ShaderType::Vertex, m_CameraData },
			{ ShaderType::Vertex, m_ObjectTransforms },
			{ ShaderType::Fragment, m_RickTexture },
			{ ShaderType::Fragment, m_PolandTexture },
			{ ShaderType::Fragment, m_TestStorageImage, true },
		});

		m_ComputeResourceLayout = ResourceLayout::Create({
			{ ShaderType::Compute, m_RickTexture },
			{ ShaderType::Compute, m_TestStorageImage },
			// { ShaderType::Compute, m_TextureAtlas },
		});
		
		m_FragmentShader = Shader::Create(ShaderType::Fragment, "Assets/Shaders/OpenGLDevelopment/FragmentShader.frag");
		m_VertexShader = Shader::Create(ShaderType::Vertex, "Assets/Shaders/OpenGLDevelopment/VertexShader.vert");
		m_ComputeShader = Shader::Create(ShaderType::Compute, "Assets/Shaders/OpenGLDevelopment/ComputeShader.comp");

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

		m_GraphicsPipeline = GraphicsPipeline::Create(graphicsPipelineCreateInfo);


		ComputePipelineCreateInfo computePipelineCreateInfo;
		computePipelineCreateInfo.ComputeShader = m_ComputeShader;
		computePipelineCreateInfo.ResourceLayout = m_ComputeResourceLayout;

		m_ComputePipeline = ComputePipeline::Create(computePipelineCreateInfo);

		FramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.Width = m_RickTexture->GetWidth();
		framebufferCreateInfo.Height = m_RickTexture->GetHeight();
		framebufferCreateInfo.SwapChainTarget = true;
		framebufferCreateInfo.Attachments = {

			FramebufferTextureFormat::RGBA8,
		};
		m_Framebuffer = Framebuffer::Create(framebufferCreateInfo);
	}
	void OpenGLDevLayer::OnDetach() {

		m_ResourceLayout->Destroy();
		m_ComputeResourceLayout->Destroy();
		m_CameraData->Destroy();
		m_ObjectTransforms->Destroy();

		m_TestStorageImage->Destroy();
		m_RickTexture->Destroy();
		m_PolandTexture->Destroy();
		// m_TextureAtlas->Destroy();

		m_IndexBuffer->Destroy();
		m_VertexBuffer->Destroy();
		m_VertexShader->Destroy();
		m_FragmentShader->Destroy();
		m_ComputeShader->Destroy();
		m_ComputePipeline->Destroy();
		m_GraphicsPipeline->Destroy();

		m_Framebuffer->Destroy();
	}

	void OpenGLDevLayer::OnUpdate(float ts) {

		m_Time += ts;
		static float avgFPS = 0;
		static uint32_t frames = 0;
		frames++;
		float FPS = 1.0f / ts;
		avgFPS += (FPS - avgFPS) / (float)frames;
		Application::GetInstance().GetActiveWindow()->SetTitle("Sandbox Application: " + std::to_string(FPS) + " FPS (avg: " + std::to_string(avgFPS) + ")" + " Time: " + std::to_string(m_Time));

		uint32_t width = Application::GetInstance().GetActiveWindow()->GetWindowData().Width;
		uint32_t height = Application::GetInstance().GetActiveWindow()->GetWindowData().Height;

		// TODO: move to function
		UBO ubo = {};
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
		glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f); // Forward into the screen goes into the negative z-direction.
		ubo.view = glm::lookAt(cameraPos, cameraPos + forward, glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.proj = glm::perspectiveFov(glm::radians(45.0f), (float)width, (float)height, 0.1f, 4000.0f);
		
		ubo.invView = glm::inverse(ubo.view);
		ubo.invProj = glm::inverse(ubo.proj);

		m_CameraData->SetData(&ubo);

		glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f*glm::sin(m_Time), 0.0f, 0.0f));
		std::vector<glm::mat4> translationMatrices = { glm::mat4(1.0f), translationMatrix};
		m_ObjectTransforms->SetData(2, translationMatrices.data());
		// END TODO

		Renderer::BeginFramebuffer(m_Framebuffer, { glm::abs(glm::sin(2.0f*m_Time)), 1.0f, 0.0f });

		m_ComputePipeline->Dispatch(m_RickTexture->GetWidth(), m_RickTexture->GetHeight(), 8, 8);

		Renderer::SetActivePipeline(m_GraphicsPipeline);
		Renderer::Draw(m_IndexBuffer, m_VertexBuffer);

		Renderer::EndFramebuffer();

	}
	void OpenGLDevLayer::OnEvent(LoFox::Event& event) {

	}
}