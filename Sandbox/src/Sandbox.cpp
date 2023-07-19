#include <LoFox.h>
#include <LoFox/Core/EntryPoint.h>

struct QuadVertex {

	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec2 TexCoord;
	uint32_t TexIndex;
};

struct ObjectData {

	alignas(16) glm::mat4 model; // Still 64 bits
};

struct UBO {

	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

const std::vector<QuadVertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, 0},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, 0},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, 0},

	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, 0},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, 0},
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, 0}
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

			m_UniformBuffer = UniformBuffer::Create(sizeof(UBO));
			m_StorageBuffer = StorageBuffer::Create(1024, sizeof(ObjectData));

			// Create texture(s)
			m_TextureAtlas = TextureAtlas::Create();
			m_Texture1 = Texture::Create("Assets/Textures/Rick.png");
			m_Texture2 = Texture::Create("Assets/Textures/poland.png");
			m_TextureAtlas->AddTexture(m_Texture1);
			m_TextureAtlas->AddTexture(m_Texture2);

			m_ResourceLayout = ResourceLayout::Create({
				{ VK_SHADER_STAGE_VERTEX_BIT,	m_UniformBuffer	},
				{ VK_SHADER_STAGE_FRAGMENT_BIT,	m_TextureAtlas	},
				{ VK_SHADER_STAGE_VERTEX_BIT,	m_StorageBuffer	},
			});

			Renderer::SetResourceLayout(m_ResourceLayout);

			// Buffers
			VertexLayout layout = { // Must match QuadVertex
				{ VertexAttributeType::Float3 }, // position
				{ VertexAttributeType::Float3 }, // color
				{ VertexAttributeType::Float2 }, // texCoord
				{ VertexAttributeType::Uint }, // texIndex
			};

			uint32_t vertexBufferSize = sizeof(vertices[0]) * vertices.size();
			m_VertexBuffer = CreateRef<VertexBuffer>(vertexBufferSize, vertices.data(), layout);

			uint32_t indexBufferSize = sizeof(vertexIndices[0]) * vertexIndices.size();
			m_IndexBuffer = CreateRef<IndexBuffer>(indexBufferSize, vertexIndices.size(), vertexIndices.data());

			// Create Graphics pipeline
			GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
			graphicsPipelineCreateInfo.VertexShaderPath = "Assets/Shaders/VertexShader.vert";
			graphicsPipelineCreateInfo.FragmentShaderPath = "Assets/Shaders/FragmentShader.frag";
			graphicsPipelineCreateInfo.ResourceLayout = m_ResourceLayout;
			GraphicsPipelineBuilder graphicsPipelineBuilder(graphicsPipelineCreateInfo);

			graphicsPipelineBuilder.PreparePushConstant(sizeof(ObjectData), VK_SHADER_STAGE_VERTEX_BIT); // model transform
			graphicsPipelineBuilder.PreparePushConstant(sizeof(uint32_t), VK_SHADER_STAGE_FRAGMENT_BIT); // Texture Index
			graphicsPipelineBuilder.PrepareVertexBuffer(m_VertexBuffer);

			m_GraphicsPipeline = graphicsPipelineBuilder.CreateGraphicsPipeline();
			Renderer::SubmitGraphicsPipeline(m_GraphicsPipeline);

			UpdateStorageBuffer();
		}
		void OnDetach() {
			
			m_VertexBuffer->Destroy();
			m_IndexBuffer->Destroy();
			m_ResourceLayout->Destroy();

			m_Texture1->Destroy();
			m_Texture2->Destroy();
			m_UniformBuffer->Destroy();
			m_StorageBuffer->Destroy();
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
			Application::GetInstance().GetActiveWindow()->SetTitle("Sandbox Application: " + std::to_string(FPS) + " FPS (avg: " + std::to_string(avgFPS) + ", max: " + std::to_string(maxFPS) + ")" + "Time: "  + std::to_string(m_Time));

			if (!Application::GetInstance().GetActiveWindow()->IsMinimized()) {

				// Can also be done set OnAttach, as long as it is before Renderer::StartFrame
				RenderCommand::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
				RenderCommand::SetViewport({ 0.0f, 0.0f }, { Renderer::GetSwapChainExtent().width, Renderer::GetSwapChainExtent().height });
				RenderCommand::SetScissor({ 0.0f, 0.0f }, { Renderer::GetSwapChainExtent().width, Renderer::GetSwapChainExtent().height});
				// Should be set before Renderer::StartFrame ----------------------------------
				UpdateUniformBuffer();
				// ----------------------------------------------------------------------------

				Renderer::StartFrame();
				
				RenderCommand::SubmitVertexBuffer(m_VertexBuffer);
				RenderCommand::SubmitIndexBuffer(m_IndexBuffer);

				uint32_t texIndex = 0;
				ObjectData modelTransform;
				modelTransform.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -50.0f, -2.3f)) * glm::rotate(glm::mat4(1.0f), m_Time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				m_GraphicsPipeline->PushConstant(0, &modelTransform);
				m_GraphicsPipeline->PushConstant(1, &texIndex);
				Renderer::Draw(1024);

				texIndex = 2; // there are only 2 textures, so this will not use a texture
				modelTransform.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -25.0f, -2.3f)) * glm::rotate(glm::mat4(1.0f), m_Time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
				m_GraphicsPipeline->PushConstant(0, &modelTransform);
				m_GraphicsPipeline->PushConstant(1, &texIndex);
				Renderer::Draw(1024);

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

		void UpdateUniformBuffer() {

			UBO ubo = {};
			ubo.view = glm::lookAt(glm::vec3(0.0f, 1.0f, 0.3f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			ubo.proj = glm::perspective(glm::radians(45.0f), Renderer::GetSwapChainExtent().width / (float)Renderer::GetSwapChainExtent().height, 0.1f, 4000.0f);
			ubo.proj[1][1] *= -1; // glm was designed for OpenGL, where the y-axis is flipped. This unflips it for Vulkan

			m_UniformBuffer->SetData(&ubo);
		}

		void UpdateStorageBuffer() {

			std::vector<ObjectData> models = {};
			models.resize(1024);
			for (uint32_t i = 0; i < 32; i++) {
				for (uint32_t j = 0; j < 32; j++) {

					glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(16.0f - j, 16.0f - i, 0.0f)) * glm::rotate(glm::mat4(1.0f), (float)(i+j)/2.0f * glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
					models[i + j * 32].model = model;
				}
			}

			m_StorageBuffer->SetData(models.data(), models.size());
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
		Ref<ResourceLayout> m_ResourceLayout;
		Ref<GraphicsPipeline> m_GraphicsPipeline;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		Ref<Texture> m_Texture1;
		Ref<Texture> m_Texture2;
		Ref<TextureAtlas> m_TextureAtlas;
		Ref<UniformBuffer> m_UniformBuffer;
		Ref<StorageBuffer> m_StorageBuffer;

		// std::vector<Ref<Buffer>> m_UniformBuffers;
		// std::vector<void*> m_UniformBuffersMapped;

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
