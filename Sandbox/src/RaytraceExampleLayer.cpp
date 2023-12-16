#include "RaytraceExampleLayer.h"
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>

namespace LoFox {

	void RaytraceExampleLayer::OnAttach() {

		m_UniformBuffer = UniformBuffer::Create(sizeof(UBO)); // Unused at the moment

		m_SphereBuffer = StorageBuffer::Create(1000, sizeof(Sphere));
		m_MaterialBuffer = StorageBuffer::Create(1000, sizeof(Material));

		uint32_t width = Application::GetInstance().GetActiveWindow()->GetWindowData().Width;
		uint32_t height = Application::GetInstance().GetActiveWindow()->GetWindowData().Height;

		m_FinalImage = StorageImage::Create(width, height);

		m_GraphicsResourceLayout = ResourceLayout::Create({
			{ ShaderType::Fragment, m_FinalImage	, true},
		});

		m_RaytraceResourceLayout = ResourceLayout::Create({
			{ ShaderType::Compute,	m_UniformBuffer	},
			{ ShaderType::Compute,	m_SphereBuffer },
			{ ShaderType::Compute,	m_MaterialBuffer },
			{ ShaderType::Compute,	m_FinalImage , false},
		});

		VertexLayout vertexLayout = { // Must match QuadVertex
			{ VertexAttributeType::Float3 }, // position
			{ VertexAttributeType::Float2 }, // texCoord
		};

		m_FragmentShader = Shader::Create(ShaderType::Fragment, "Assets/Shaders/RaytraceExample/RTFragmentShader.frag");
		m_VertexShader = Shader::Create(ShaderType::Vertex, "Assets/Shaders/RaytraceExample/RTVertexShader.vert");
		m_ComputeShader = Shader::Create(ShaderType::Compute, "Assets/Shaders/RaytraceExample/RTComputeShader.comp");
		// m_ComputeShader = Shader::Create(ShaderType::Compute, "Assets/Shaders/RaytraceExample/RTComputeShader2.comp");

		uint32_t vertexBufferSize = sizeof(vertices[0]) * vertices.size();
		m_VertexBuffer = VertexBuffer::Create(vertexBufferSize, vertices.data(), vertexLayout);
		m_IndexBuffer = IndexBuffer::Create(vertexIndices.size(), vertexIndices.data());

		// Create Compute Pipeline
		ComputePipelineCreateInfo computePipelineCreateInfo = {};
		computePipelineCreateInfo.ComputeShader = m_ComputeShader;
		computePipelineCreateInfo.ResourceLayout = m_RaytraceResourceLayout;
		m_RaytracePipeline = ComputePipeline::Create(computePipelineCreateInfo);

		// Create Graphics pipeline
		GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
		graphicsPipelineCreateInfo.VertexShader = m_VertexShader;
		graphicsPipelineCreateInfo.FragmentShader = m_FragmentShader;
		graphicsPipelineCreateInfo.ResourceLayout = m_GraphicsResourceLayout;
		graphicsPipelineCreateInfo.Topology = Topology::Triangle;
		graphicsPipelineCreateInfo.VertexLayout = vertexLayout;
		m_GraphicsPipeline = GraphicsPipeline::Create(graphicsPipelineCreateInfo);

		FramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.Width = width;
		framebufferCreateInfo.Height = height;
		framebufferCreateInfo.SwapChainTarget = false;
		framebufferCreateInfo.Attachments = {

			FramebufferTextureFormat::RGBA8,
		};
		m_Framebuffer = Framebuffer::Create(framebufferCreateInfo);

		SetStorageBuffers();
	}

	void RaytraceExampleLayer::OnDetach() {

		m_VertexShader->Destroy();
		m_FragmentShader->Destroy();
		m_ComputeShader->Destroy();

		m_VertexBuffer->Destroy();
		m_IndexBuffer->Destroy();
		m_GraphicsResourceLayout->Destroy();
		m_RaytraceResourceLayout->Destroy();

		m_UniformBuffer->Destroy();
		m_SphereBuffer->Destroy();
		m_MaterialBuffer->Destroy();
		m_FinalImage->Destroy();

		m_RaytracePipeline->Destroy(); // All pipelines other than the graphicspipeline provided to the Renderer must be destroyed
		m_GraphicsPipeline->Destroy();

		m_Framebuffer->Destroy();
	}

	void RaytraceExampleLayer::OnUpdate(float ts) {

		m_Time += ts;
		static float avgFPS = 0;
		static uint32_t frames = 0;
		frames++;
		float FPS = 1.0f / ts;
		avgFPS += (FPS - avgFPS) / (float)frames;
		Application::GetInstance().GetActiveWindow()->SetTitle("Sandbox Application: " + std::to_string(FPS) + " FPS (avg: " + std::to_string(avgFPS) + ")" + " Time: " + std::to_string(m_Time));

		if (!m_ViewportWidth == 0 && !m_ViewportHeight == 0) { // Only render when viewport is non-zero (minimizing viewport, ...)

			UpdateUniformBuffer();
			SetStorageBuffers();

			uint32_t width = m_Framebuffer->GetWidth();
			uint32_t height = m_Framebuffer->GetHeight();

			m_RaytracePipeline->Dispatch(width, height, 8, 8);

			Renderer::BeginFramebuffer(m_Framebuffer, { 1.0f, 0.0f, 0.0f });
			
			
			Renderer::SetActivePipeline(m_GraphicsPipeline);
			Renderer::Draw(m_IndexBuffer, m_VertexBuffer);
			
			Renderer::EndFramebuffer();

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

	void RaytraceExampleLayer::OnImGuiRender() {

		{ // Settings
			ImGui::Begin("Settings");
			ImGui::Text("This is the settings window");

			ImGui::Separator();

			ImGui::End();
		}

		{ // Viewport
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport");

			ImVec2 extent = ImGui::GetContentRegionAvail();
			m_ViewportWidth = extent.x;
			m_ViewportHeight = extent.y;

			uint64_t textureID = *(uint64_t*)m_Framebuffer->GetAttachmentImTextureID(0);
			ImGui::Image((void*)textureID, extent, ImVec2{0, 1}, ImVec2{1, 0});

			ImGui::PopStyleVar();

			ImGui::End();
		}
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

		uint32_t width = m_ViewportWidth;
		uint32_t height = m_ViewportHeight;

		UBO ubo = {};
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -5.0f);
		glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f); // Forward into the screen goes into the negative z-direction.
		ubo.view = glm::lookAt(cameraPos, cameraPos + forward, glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.proj = glm::perspectiveFov(glm::radians(45.0f), (float)width, (float)height, 0.1f, 4000.0f);

		ubo.invView = glm::inverse(ubo.view);
		ubo.invProj = glm::inverse(ubo.proj);

		ubo.Time = m_Time;
		ubo.FrameIndex = m_FrameIndex;

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

		m_SphereBuffer->SetData(spheres.size(), spheres.data());

		// Materials
		std::vector<Material> materials = {};
		materials.resize(3);
		materials[0].Albedo = glm::vec3(1.0f, 1.0f, 0.0f);
		materials[0].Roughness = 0.05f;
		materials[0].Metallic = 0.2f;
		materials[0].EmissionColor = glm::vec3(1.0f, 1.0f, 0.0f);
		materials[0].EmissionPower = 0.0f;

		materials[1].Albedo = glm::vec3(1.0f, 0.0f, 1.0f);
		materials[1].Roughness = 0.1f;
		materials[1].Metallic = 0.2f;
		materials[1].EmissionColor = glm::vec3(1.0f, 0.0f, 1.0f);
		materials[1].EmissionPower = 0.0f;

		materials[2].Albedo = glm::vec3(0.0f, 1.0f, 1.0f);
		materials[2].Roughness = 0.1f;
		materials[2].Metallic = 0.2f;
		materials[2].EmissionColor = glm::vec3(1.0f, 0.7f, 0.3f);
		materials[2].EmissionPower = 1.0f;

		m_MaterialBuffer->SetData(materials.size(), materials.data());
	}
}