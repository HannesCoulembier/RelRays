#include "RaytraceExampleLayer.h"

namespace LoFox {

	void RaytraceExampleLayer::OnAttach() {

		m_UniformBuffer = UniformBuffer::Create(sizeof(UBO)); // Unused at the moment

		m_FinalImage = StorageImage::Create(Renderer::GetSwapChainExtent().width, Renderer::GetSwapChainExtent().height);

		m_GraphicsResourceLayout = ResourceLayout::Create({
			{ VK_SHADER_STAGE_FRAGMENT_BIT, m_FinalImage	, true}, // Is destination -> isDestination = true
			});

		m_RaytraceResourceLayout = ResourceLayout::Create({
			{ VK_SHADER_STAGE_COMPUTE_BIT,	m_UniformBuffer	},
			{ VK_SHADER_STAGE_COMPUTE_BIT,	m_FinalImage , false}, // Is source -> isDestination = false
			});

		Renderer::SetResourceLayout(m_GraphicsResourceLayout);

		// Buffers
		VertexLayout layout = { // Must match QuadVertex
			{ VertexAttributeType::Float3 }, // position
			{ VertexAttributeType::Float2 }, // texCoord
		};

		uint32_t vertexBufferSize = sizeof(vertices[0]) * vertices.size();
		m_VertexBuffer = CreateRef<VertexBuffer>(vertexBufferSize, vertices.data(), layout);

		uint32_t indexBufferSize = sizeof(vertexIndices[0]) * vertexIndices.size();
		m_IndexBuffer = CreateRef<IndexBuffer>(indexBufferSize, vertexIndices.size(), vertexIndices.data());

		// Create Compute Pipeline
		ComputePipelineCreateInfo computePipelineCreateInfo = {};
		computePipelineCreateInfo.ComputeShaderPath = "Assets/Shaders/RaytraceExample/RTComputeShader.comp";
		computePipelineCreateInfo.ResourceLayout = m_RaytraceResourceLayout;

		ComputePipelineBuilder computePipelineBuilder(computePipelineCreateInfo);

		m_RaytracePipeline = computePipelineBuilder.CreateComputePipeline();

		// Create Graphics pipeline
		GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
		graphicsPipelineCreateInfo.VertexShaderPath = "Assets/Shaders/RaytraceExample/RTVertexShader.vert";
		graphicsPipelineCreateInfo.FragmentShaderPath = "Assets/Shaders/RaytraceExample/RTFragmentShader.frag";
		graphicsPipelineCreateInfo.ResourceLayout = m_GraphicsResourceLayout;
		GraphicsPipelineBuilder graphicsPipelineBuilder(graphicsPipelineCreateInfo);

		graphicsPipelineBuilder.PrepareVertexBuffer(m_VertexBuffer);

		m_GraphicsPipeline = graphicsPipelineBuilder.CreateGraphicsPipeline();
		Renderer::SubmitGraphicsPipeline(m_GraphicsPipeline);
	}

	void RaytraceExampleLayer::OnDetach() {

		m_VertexBuffer->Destroy();
		m_IndexBuffer->Destroy();
		m_GraphicsResourceLayout->Destroy();
		m_RaytraceResourceLayout->Destroy();

		m_UniformBuffer->Destroy();
		m_FinalImage->Destroy();

		m_RaytracePipeline->Destroy(); // All pipelines other than the graphicspipeline provided to the Renderer must be destroyed
	}

	void RaytraceExampleLayer::OnUpdate(float ts) {

		m_Time += ts;
		static float avgFPS = 0;
		static uint32_t frames = 0;
		frames++;
		float FPS = 1.0f / ts;
		avgFPS += (FPS - avgFPS) / (float)frames;
		Application::GetInstance().GetActiveWindow()->SetTitle("Sandbox Application: " + std::to_string(FPS) + " FPS (avg: " + std::to_string(avgFPS) + ")" + " Time: " + std::to_string(m_Time));

		if (!Application::GetInstance().GetActiveWindow()->IsMinimized()) {

			// Can also be done set OnAttach, as long as it is before Renderer::StartFrame
			RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
			RenderCommand::SetViewport({ 0.0f, 0.0f }, { Renderer::GetSwapChainExtent().width, Renderer::GetSwapChainExtent().height });
			RenderCommand::SetScissor({ 0.0f, 0.0f }, { Renderer::GetSwapChainExtent().width, Renderer::GetSwapChainExtent().height });
			// Should be set before Renderer::StartFrame ----------------------------------
			UpdateUniformBuffer();
			// ----------------------------------------------------------------------------

			Renderer::PrepareFrame();

			m_RaytracePipeline->Bind();
			m_RaytracePipeline->Dispatch(Renderer::GetSwapChainExtent().width, Renderer::GetSwapChainExtent().height, 8, 8);

			Renderer::StartFrame();

			RenderCommand::SubmitVertexBuffer(m_VertexBuffer);
			RenderCommand::SubmitIndexBuffer(m_IndexBuffer);

			Renderer::Draw(1);

			Renderer::SubmitFrame();
		}

		// Logger test
		/*
		LF_CORE_TRACE("TEST");
		LF_CORE_INFO("TEST");
		LF_CORE_WARN("TEST");
		LF_CORE_ERROR("TEST");
		LF_CORE_CRITICAL("TEST");

		LF_TRACE("TEST");
		LF_INFO("TEST");
		LF_WARN("TEST");
		LF_ERROR("TEST");
		LF_CRITICAL("TEST");
		*/

		// Input test
		/*
		LF_INFO("{0}", LoFox::Input::IsKeyPressed(LoFox::KeyCode::A));
		LF_INFO("{0}", LoFox::Input::GetMousePosition());
		LF_INFO("{0}", LoFox::Input::IsMouseButtonPressed(LoFox::MouseCode::Button0));
		*/
	}

	void RaytraceExampleLayer::OnEvent(Event& event) {

		LoFox::EventDispatcher dispatcher(event);

		// Event test
		/*
		if (event.GetEventType() == LoFox::EventType::KeyPressed)
			LF_INFO("Pressed key {0}", static_cast<LoFox::KeyPressedEvent&>(event).GetKeyCode());
		*/
	}

	void RaytraceExampleLayer::UpdateUniformBuffer() {

		UBO ubo = {};
		ubo.view = glm::lookAt(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), Renderer::GetSwapChainExtent().width / (float)Renderer::GetSwapChainExtent().height, 0.1f, 4000.0f);
		ubo.proj[1][1] *= -1; // glm was designed for OpenGL, where the y-axis is flipped. This unflips it for Vulkan

		ubo.invView = glm::inverse(ubo.view);
		ubo.invProj = glm::inverse(ubo.proj);

		m_UniformBuffer->SetData(&ubo);
	}
}