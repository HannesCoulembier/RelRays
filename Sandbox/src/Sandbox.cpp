#include <LoFox.h>
#include <LoFox/Core/EntryPoint.h>

struct QuadVertex {

	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec2 TexCoord;
};

struct ObjectData {

	glm::mat4 model;
};

const std::vector<QuadVertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint32_t> vertexIndices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4,
};

namespace LoFox {

	class ExampleLayer : public Layer {

	public:
		ExampleLayer() {}
		~ExampleLayer() {}

		void OnAttach() {

			// Create GraphicsDescriptorset layouts
			VkDescriptorSetLayoutBinding uboLayoutBinding = {}; // uniform buffer with MVP matrices
			uboLayoutBinding.binding = 0;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			uboLayoutBinding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
			samplerLayoutBinding.binding = 1;
			samplerLayoutBinding.descriptorCount = 1;
			samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			samplerLayoutBinding.pImmutableSamplers = nullptr;
			samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			std::vector<VkDescriptorSetLayoutBinding> graphicsBindings = { uboLayoutBinding, samplerLayoutBinding };
			VkDescriptorSetLayoutCreateInfo graphicsDescriptorSetlayoutCreateInfo = {};
			graphicsDescriptorSetlayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			graphicsDescriptorSetlayoutCreateInfo.bindingCount = (uint32_t)graphicsBindings.size();
			graphicsDescriptorSetlayoutCreateInfo.pBindings = graphicsBindings.data();

			LF_CORE_ASSERT(vkCreateDescriptorSetLayout(RenderContext::LogicalDevice, &graphicsDescriptorSetlayoutCreateInfo, nullptr, &m_GraphicsDescriptorSetLayout) == VK_SUCCESS, "Failed to create descriptor set layout!");

			// Buffers
			VertexLayout layout = { // Must match QuadVertex
				{ VertexAttributeType::Float3 }, // position
				{ VertexAttributeType::Float3 }, // color
				{ VertexAttributeType::Float2 }, // texCoord
			};

			uint32_t vertexBufferSize = sizeof(vertices[0]) * vertices.size();
			m_VertexBuffer = CreateRef<VertexBuffer>(vertexBufferSize, vertices.data(), layout);

			uint32_t indexBufferSize = sizeof(vertexIndices[0]) * vertexIndices.size();
			m_IndexBuffer = CreateRef<IndexBuffer>(indexBufferSize, vertexIndices.size(), vertexIndices.data());

			// Create Graphics pipeline
			GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
			graphicsPipelineCreateInfo.VertexShaderPath = "Assets/Shaders/VertexShader.vert";
			graphicsPipelineCreateInfo.FragmentShaderPath = "Assets/Shaders/FragmentShader.frag";
			graphicsPipelineCreateInfo.DescriptorSetLayout = m_GraphicsDescriptorSetLayout;
			GraphicsPipelineBuilder graphicsPipelineBuilder(graphicsPipelineCreateInfo);

			graphicsPipelineBuilder.PreparePushConstant(sizeof(ObjectData), VK_SHADER_STAGE_VERTEX_BIT);
			graphicsPipelineBuilder.PrepareVertexBuffer(m_VertexBuffer);

			m_GraphicsPipeline = graphicsPipelineBuilder.CreateGraphicsPipeline();
			Renderer::SubmitGraphicsPipeline(m_GraphicsPipeline);
		}
		void OnDetach() {
			
			m_VertexBuffer->Destroy();
			m_IndexBuffer->Destroy();
			vkDestroyDescriptorSetLayout(RenderContext::LogicalDevice, m_GraphicsDescriptorSetLayout, nullptr);
		}

		void OnUpdate(float ts) {

			m_Time += ts;
			static float avgFPS = 0;
			static uint32_t frames = 0;
			frames++;
			float FPS = 1.0f / ts;
			avgFPS += (FPS - avgFPS) / (float)frames;
			static float maxFPS = 0;
			maxFPS = std::max(maxFPS, FPS);
			Application::GetInstance().GetActiveWindow()->SetTitle("Sandbox Application: " + std::to_string(FPS) + " FPS (avg: " + std::to_string(avgFPS) + ", max: " + std::to_string(maxFPS) + ")");

			if (!Application::GetInstance().GetActiveWindow()->IsMinimized()) {

				// Can also be done set OnAttach, as long as it is before Renderer::StartFrame
				RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
				RenderCommand::SetViewport({ 0.0f, 0.0f }, { Renderer::GetSwapChainExtent().width, Renderer::GetSwapChainExtent().height });
				RenderCommand::SetScissor({ 0.0f, 0.0f }, { Renderer::GetSwapChainExtent().width, Renderer::GetSwapChainExtent().height});
				// ----------------------------------------------------------------------------

				Renderer::StartFrame();
				
				RenderCommand::SubmitVertexBuffer(m_VertexBuffer);
				RenderCommand::SubmitIndexBuffer(m_IndexBuffer);

				ObjectData test;
				test.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, -0.2f)) * glm::rotate(glm::mat4(1.0f), m_Time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				m_GraphicsPipeline->PushConstant(0, &test);
				
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

		void OnEvent(LoFox::Event& event) {

			LoFox::EventDispatcher dispatcher(event);
			
			// Event test
			/*
			if (event.GetEventType() == LoFox::EventType::KeyPressed)
				LF_INFO("Pressed key {0}", static_cast<LoFox::KeyPressedEvent&>(event).GetKeyCode());
			*/
		}

	private:
		VkDescriptorSetLayout m_GraphicsDescriptorSetLayout;
		Ref<GraphicsPipeline> m_GraphicsPipeline;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		float m_Time = 0;
	};

	class SandboxApp : public Application {

	public:
		SandboxApp(const ApplicationSpec& spec)
			: Application(spec) {
			
			PushLayer(CreateRef<ExampleLayer>());
		}

		~SandboxApp() {

		}
	};

	Application* CreateApplication() {

		ApplicationSpec spec;
		spec.Name = "Sandbox Application";
		return new SandboxApp(spec);
	}
}
