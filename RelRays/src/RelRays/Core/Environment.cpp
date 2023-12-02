#include "RelRays/Core/Environment.h"

namespace RelRays {

	// TEMPORARY STUFF FROM RAYTRACE EXAMPLE
	struct UBO {

		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::mat4 invView;
		alignas(16) glm::mat4 invProj;
		alignas(4) float Time;
		alignas(4) int FrameIndex;
	};

	struct Sphere {
		alignas(16) glm::vec3 Position;
		alignas(4) float Radius;
		alignas(4) int MaterialIndex = 0;
	};

	struct GPUObjectDescription {

		alignas(8) int LatestFragmentIndex;
		alignas(4) int NumberOfFragments;

		alignas(4) float Radius; // To be replaced with mesh ID
	};

	struct GPUObjectFragment {

		alignas(16) glm::vec3 Pos; // To be extended

		alignas(4) int MaterialIndex;
	};

	struct Material {
		alignas(16) glm::vec3 Albedo;
		alignas(4) float Roughness;
		alignas(4) float Metallic;
		alignas(16) glm::vec3 EmissionColor;
		alignas(4) float EmissionPower;
	};

	void Environment::Init(EnvironmentCreateInfo createInfo) {

		m_CreateInfo = createInfo;

		// TEMPORARY STUFF FROM RAYTRACE EXAMPLE
		m_UniformBuffer = LoFox::UniformBuffer::Create(sizeof(UBO)); // Unused at the moment

		m_ObjectDescriptionBuffer = LoFox::StorageBuffer::Create(1000, sizeof(GPUObjectDescription));
		m_ObjectFragmentsBuffer = LoFox::StorageBuffer::Create(1000, sizeof(GPUObjectFragment));

		m_MaterialBuffer = LoFox::StorageBuffer::Create(1000, sizeof(Material));

		uint32_t width = LoFox::Application::GetInstance().GetActiveWindow()->GetWindowData().Width;
		uint32_t height = LoFox::Application::GetInstance().GetActiveWindow()->GetWindowData().Height;

		m_FinalImageRenderData.FinalImage = LoFox::StorageImage::Create(width, height);

		m_FinalImageRenderData.GraphicsResourceLayout = LoFox::ResourceLayout::Create({
			{ LoFox::ShaderType::Fragment, m_FinalImageRenderData.FinalImage	, true},
		});

		m_RaytraceRendererData.RaytraceResourceLayout = LoFox::ResourceLayout::Create({
			{ LoFox::ShaderType::Compute,	m_UniformBuffer	},
			{ LoFox::ShaderType::Compute,	m_ObjectDescriptionBuffer },
			{ LoFox::ShaderType::Compute,	m_ObjectFragmentsBuffer },
			{ LoFox::ShaderType::Compute,	m_MaterialBuffer },
			{ LoFox::ShaderType::Compute,	m_FinalImageRenderData.FinalImage , false},
		});

		LoFox::VertexLayout vertexLayout = { // Must match QuadVertex
			{ LoFox::VertexAttributeType::Float3 }, // position
			{ LoFox::VertexAttributeType::Float2 }, // texCoord
		};

		m_FinalImageRenderData.FragmentShader = LoFox::Shader::Create(LoFox::ShaderType::Fragment, "Assets/Shaders/MainFragmentShader.frag");
		m_FinalImageRenderData.VertexShader = LoFox::Shader::Create(LoFox::ShaderType::Vertex, "Assets/Shaders/MainVertexShader.vert");
		m_RaytraceRendererData.ComputeShader = LoFox::Shader::Create(LoFox::ShaderType::Compute, "Assets/Shaders/MainComputeShader.comp");
		// m_ComputeShader = Shader::Create(ShaderType::Compute, "Assets/Shaders/RaytraceExample/RTComputeShader2.comp");

		uint32_t vertexBufferSize = sizeof(m_FinalImageRenderData.vertices[0]) * m_FinalImageRenderData.vertices.size();
		m_FinalImageRenderData.VertexBuffer = LoFox::VertexBuffer::Create(vertexBufferSize, m_FinalImageRenderData.vertices.data(), vertexLayout);
		m_FinalImageRenderData.IndexBuffer = LoFox::IndexBuffer::Create(m_FinalImageRenderData.vertexIndices.size(), m_FinalImageRenderData.vertexIndices.data());

		// Create Compute Pipeline
		LoFox::ComputePipelineCreateInfo computePipelineCreateInfo = {};
		computePipelineCreateInfo.ComputeShader = m_RaytraceRendererData.ComputeShader;
		computePipelineCreateInfo.ResourceLayout = m_RaytraceRendererData.RaytraceResourceLayout;
		m_RaytraceRendererData.RaytracePipeline = LoFox::ComputePipeline::Create(computePipelineCreateInfo);

		// Create Graphics pipeline
		LoFox::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
		graphicsPipelineCreateInfo.VertexShader = m_FinalImageRenderData.VertexShader;
		graphicsPipelineCreateInfo.FragmentShader = m_FinalImageRenderData.FragmentShader;
		graphicsPipelineCreateInfo.ResourceLayout = m_FinalImageRenderData.GraphicsResourceLayout;
		graphicsPipelineCreateInfo.Topology = LoFox::Topology::Triangle;
		graphicsPipelineCreateInfo.VertexLayout = vertexLayout;
		m_FinalImageRenderData.GraphicsPipeline = LoFox::GraphicsPipeline::Create(graphicsPipelineCreateInfo);

		SetStorageBuffers();

	}

	LoFox::Ref<Environment> Environment::Create(EnvironmentCreateInfo createInfo) {

		LoFox::Ref<Environment> env = LoFox::CreateRef<Environment>();
		env->SetSelf(env);
		env->Init(createInfo);
		return env;
	}

	void Environment::SetSelf(LoFox::Ref<Environment> env) {

		m_Self = env;
	}

	void Environment::OnUpdate() {

		float ts;
		if (m_CreateInfo.UseConstantTimeStep) {
			ts = m_CreateInfo.ConstantTimeStepValue;
			m_SimulationTime += ts;
		}
		else {
			float time = LoFox::Time::GetTime();
			ts = time - m_SimulationTime;
			m_SimulationTime = time;
		}
	}

	void Environment::RenderFrame() {

		UpdateUniformBuffer();
		SetStorageBuffers();

		uint32_t width = LoFox::Application::GetInstance().GetActiveWindow()->GetWindowData().Width;
		uint32_t height = LoFox::Application::GetInstance().GetActiveWindow()->GetWindowData().Height;

		LoFox::Renderer::BeginFrame({ 1.0f, 0.0f, 1.0f });

		m_RaytraceRendererData.RaytracePipeline->Dispatch(width, height, 8, 8);

		LoFox::Renderer::SetActivePipeline(m_FinalImageRenderData.GraphicsPipeline);
		LoFox::Renderer::Draw(m_FinalImageRenderData.IndexBuffer, m_FinalImageRenderData.VertexBuffer);

		LoFox::Renderer::EndFrame();
		m_FrameIndex++;
	}

	void Environment::Destroy() {

		// Shaders
		m_FinalImageRenderData.VertexShader->Destroy();
		m_FinalImageRenderData.FragmentShader->Destroy();
		m_RaytraceRendererData.ComputeShader->Destroy();

		// Render stuff
		m_FinalImageRenderData.VertexBuffer->Destroy();
		m_FinalImageRenderData.IndexBuffer->Destroy();
		m_FinalImageRenderData.GraphicsResourceLayout->Destroy();
		m_RaytraceRendererData.RaytraceResourceLayout->Destroy();
		m_FinalImageRenderData.FinalImage->Destroy();

		// Buffers
		m_UniformBuffer->Destroy();
		m_ObjectDescriptionBuffer->Destroy();
		m_ObjectFragmentsBuffer->Destroy();
		m_MaterialBuffer->Destroy();

		// Pipelines
		m_RaytraceRendererData.RaytracePipeline->Destroy(); // All pipelines other than the graphicspipeline provided to the Renderer must be destroyed
		m_FinalImageRenderData.GraphicsPipeline->Destroy();

		// Empty the whole environment (this way any shared_ptrs should get released)
		m_Self = nullptr;
		m_Objects = {};
	}

	LoFox::Ref<Object> Environment::CreateObject(glm::vec3 pos, float radius) {

		LoFox::Ref<Object> object = LoFox::CreateRef<Object>(m_Self, pos, radius);
		m_Objects.push_back(object);
		return object;
	}


	// TEMPORARY STUFF FROM RAYTRACE EXAMPLE
	void Environment::UpdateUniformBuffer() {

		uint32_t width = LoFox::Application::GetInstance().GetActiveWindow()->GetWindowData().Width;
		uint32_t height = LoFox::Application::GetInstance().GetActiveWindow()->GetWindowData().Height;

		UBO ubo = {};
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -5.0f - 2.0f * glm::sin(m_SimulationTime));
		glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f); // Forward into the screen goes into the negative z-direction.
		ubo.view = glm::lookAt(cameraPos, cameraPos + forward, glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.proj = glm::perspectiveFov(glm::radians(45.0f), (float)width, (float)height, 0.1f, 4000.0f);
		// Next line is not nescessary as we are using the projection matrix in our own compute shader instead of in vulkan directly
		// ubo.proj[1][1] *= -1; // glm was designed for OpenGL, where the y-axis is flipped. This unflips it for Vulkan

		ubo.invView = glm::inverse(ubo.view);
		ubo.invProj = glm::inverse(ubo.proj);

		ubo.Time = m_SimulationTime;
		ubo.FrameIndex = m_FrameIndex;

		m_UniformBuffer->SetData(&ubo);
	}

	void Environment::SetStorageBuffers() {

		std::vector<GPUObjectDescription> objectDescriptions = {};
		std::vector<GPUObjectFragment> objectFragments = {};
		for (auto object : m_Objects) {

			GPUObjectDescription description = {};
			description.LatestFragmentIndex = objectFragments.size();

			{
				GPUObjectFragment fragment = {};
				fragment.MaterialIndex = 2;
				fragment.Pos = object->m_Pos / Units::m; // GPU shader uses meters

				objectFragments.push_back(fragment);
			}

			description.NumberOfFragments = 1;
			description.Radius = object->m_Radius / Units::m; // GPU shader uses meters

			objectDescriptions.push_back(description);
		}

		m_ObjectDescriptionBuffer->SetData(objectDescriptions.size(), objectDescriptions.data());
		m_ObjectFragmentsBuffer->SetData(objectFragments.size(), objectFragments.data());



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