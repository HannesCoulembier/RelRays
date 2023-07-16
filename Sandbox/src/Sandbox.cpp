#include <LoFox.h>
#include <LoFox/Core/EntryPoint.h>

struct Vertex {

	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec2 TexCoord;

	static VkVertexInputBindingDescription GetBindingDescription() {

		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, Color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, TexCoord);

		return attributeDescriptions;
	}
};

struct ObjectData {

	glm::mat4 model;
};

const std::vector<Vertex> vertices = {
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

			// Create Graphics pipeline
			GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
			graphicsPipelineCreateInfo.VertexShaderPath = "Assets/Shaders/VertexShader.vert";
			graphicsPipelineCreateInfo.FragmentShaderPath = "Assets/Shaders/FragmentShader.frag";
			graphicsPipelineCreateInfo.VertexAttributeDescriptions = Vertex::GetAttributeDescriptions();
			graphicsPipelineCreateInfo.VertexBindingDescription = Vertex::GetBindingDescription();
			graphicsPipelineCreateInfo.DescriptorSetLayout = m_GraphicsDescriptorSetLayout;

			GraphicsPipelineBuilder graphicsPipelineBuilder(graphicsPipelineCreateInfo);
			graphicsPipelineBuilder.PreparePushConstant(sizeof(ObjectData), VK_SHADER_STAGE_VERTEX_BIT);
			m_GraphicsPipeline = graphicsPipelineBuilder.CreateGraphicsPipeline();

			Renderer::SubmitGraphicsPipeline(m_GraphicsPipeline);

			uint32_t vertexBufferSize = sizeof(vertices[0]) * vertices.size();
			m_VertexBuffer = CreateRef<VertexBuffer>(vertexBufferSize, vertices.data());

			uint32_t indexBufferSize = sizeof(vertexIndices[0]) * vertexIndices.size();
			m_IndexBuffer = CreateRef<IndexBuffer>(indexBufferSize, vertexIndices.size(), vertexIndices.data());
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
				test.model = glm::translate(glm::mat4(1.0f), glm::vec3(-0.3f)) * glm::rotate(glm::mat4(1.0f), m_Time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
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
