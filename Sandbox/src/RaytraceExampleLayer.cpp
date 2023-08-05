#include "RaytraceExampleLayer.h"

#include <glm/gtc/matrix_transform.hpp>

namespace LoFox {

	void RaytraceExampleLayer::OnAttach() {

		m_UniformBuffer = UniformBuffer::Create(sizeof(UBO)); // Unused at the moment

		m_SphereBuffer = StorageBuffer::Create(3, sizeof(Sphere));
		m_MaterialBuffer = StorageBuffer::Create(3, sizeof(Material));

		m_FinalImage = StorageImage::Create(Renderer::GetSwapChainExtent().width, Renderer::GetSwapChainExtent().height);

		m_GraphicsResourceLayout = ResourceLayout::Create({
			{ ShaderType::Fragment, m_FinalImage	, true}, // Is destination -> isDestination = true
		});

		m_RaytraceResourceLayout = ResourceLayout::Create({
			{ ShaderType::Compute,	m_UniformBuffer	},
			{ ShaderType::Compute,	m_SphereBuffer },
			{ ShaderType::Compute,	m_MaterialBuffer },
			{ ShaderType::Compute,	m_FinalImage , false}, // Is source -> isDestination = false
		});

		Sphere testSphere;
		testSphere.Position = glm::vec3(0.0);
		testSphere.Radius = 1.0f;

		Material testMat;
		testMat.Albedo = glm::vec3(1.0f, 0.0f, 0.0f);
		testMat.EmissionColor = glm::vec3(0.0);
		testMat.EmissionPower = 0.0f;
		testMat.Metallic = 0.0f;
		testMat.Roughness = 0.0f;

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

		computePipelineBuilder.PreparePushConstant(sizeof(PushConstantObject), ShaderType::Compute);

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

		SetStorageBuffers();
	}

	void RaytraceExampleLayer::OnDetach() {

		m_VertexBuffer->Destroy();
		m_IndexBuffer->Destroy();
		m_GraphicsResourceLayout->Destroy();
		m_RaytraceResourceLayout->Destroy();

		m_UniformBuffer->Destroy();
		m_SphereBuffer->Destroy();
		m_MaterialBuffer->Destroy();
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

			PushConstantObject pushConstant;
			pushConstant.Time = m_Time;
			pushConstant.FrameIndex = m_FrameIndex;
			m_RaytracePipeline->PushConstant(0, &pushConstant);
			m_RaytracePipeline->Dispatch(Renderer::GetSwapChainExtent().width, Renderer::GetSwapChainExtent().height, 8, 8);

			Renderer::StartFrame();

			RenderCommand::SubmitVertexBuffer(m_VertexBuffer);
			RenderCommand::SubmitIndexBuffer(m_IndexBuffer);

			Renderer::Draw(1);

			Renderer::SubmitFrame();
			m_FrameIndex++;
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
		glm::vec3 cameraPos = glm::vec3(0.0, 0.0, -5.0f);
		glm::vec3 forward = glm::vec3(0.0, 0.0, -1.0f); // Forward into the screen goes into the negative z-direction.
		ubo.view = glm::lookAt(cameraPos, cameraPos + forward, glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.proj = glm::perspectiveFov(glm::radians(45.0f), (float)Renderer::GetSwapChainExtent().width, (float)Renderer::GetSwapChainExtent().height, 0.1f, 4000.0f);
		// Next line is not nescessary as we are using the projection matrix in our own compute shader instead of in vulkan directly
		// ubo.proj[1][1] *= -1; // glm was designed for OpenGL, where the y-axis is flipped. This unflips it for Vulkan

		ubo.invView = glm::inverse(ubo.view);
		ubo.invProj = glm::inverse(ubo.proj);

		m_UniformBuffer->SetData(&ubo);
	}

	void RaytraceExampleLayer::SetStorageBuffers() {

		// Spheres
		std::vector<Sphere> spheres = {};
		spheres.resize(3);
		spheres[0].Position = glm::vec3(0.0f, -0.135f, 0.0f);
		spheres[0].Radius = 1.0f;
		spheres[0].MaterialIndex = 0;

		spheres[1].Position = glm::vec3(2.0f, 1.0f, 0.0f);
		spheres[1].Radius = 1.0f;
		spheres[1].MaterialIndex = 2;

		spheres[2].Position = glm::vec3(1.0f, -101.0f, -5.0f);
		spheres[2].Radius = 100.0f;
		spheres[2].MaterialIndex = 1;

		m_SphereBuffer->SetData(spheres.data(), spheres.size());

		// Materials
		std::vector<Material> materials = {};
		materials.resize(3);
		materials[0].Albedo = glm::vec3(1.0f, 1.0f, 0.0f);
		materials[0].Roughness = 0.05f;
		materials[0].Metallic = 0.0f;
		materials[0].EmissionColor = glm::vec3(1.0f, 1.0f, 0.0f);
		materials[0].EmissionPower = 0.0f;

		materials[1].Albedo = glm::vec3(1.0f, 0.0f, 1.0f);
		materials[1].Roughness = 0.1f;
		materials[1].Metallic = 0.0f;
		materials[1].EmissionColor = glm::vec3(1.0f, 0.0f, 1.0f);
		materials[1].EmissionPower = 0.0f;

		materials[2].Albedo = glm::vec3(1.0f, 0.7f, 0.3f);
		materials[2].Roughness = 0.1f;
		materials[2].Metallic = 0.0f;
		materials[2].EmissionColor = glm::vec3(1.0f, 0.7f, 0.3f);
		materials[2].EmissionPower = 1.0f;

		m_MaterialBuffer->SetData(materials.data(), materials.size());
	}
}