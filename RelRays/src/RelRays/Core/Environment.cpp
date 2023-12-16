#include "RelRays/Core/Environment.h"

#include "RelRays/Core/Defaults.h"

namespace RelRays {

	// TEMPORARY STUFF FROM RAYTRACE EXAMPLE
	struct UBO {

		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::mat4 invView;
		alignas(16) glm::mat4 invProj;
		alignas(4) float Time;
	};

	struct GPUColorSpectraDescription {

		alignas(4) int		InfraredSpectrumIndex;
		alignas(4) int		InfraredSpectrumSize;
		alignas(4) float	InfraredSpectrumMinWaveLength;
		alignas(4) float	InfraredSpectrumMaxWaveLength;

		alignas(4) int		RedSpectrumIndex;
		alignas(4) int		RedSpectrumSize;
		alignas(4) float	RedSpectrumMinWaveLength;
		alignas(4) float	RedSpectrumMaxWaveLength;

		alignas(4) int		GreenSpectrumIndex;
		alignas(4) int		GreenSpectrumSize;
		alignas(4) float	GreenSpectrumMinWaveLength;
		alignas(4) float	GreenSpectrumMaxWaveLength;

		alignas(4) int		BlueSpectrumIndex;
		alignas(4) int		BlueSpectrumSize;
		alignas(4) float	BlueSpectrumMinWaveLength;
		alignas(4) float	BlueSpectrumMaxWaveLength;

		alignas(16) glm::vec4 RelativeWeights;
	};

	struct GPUCameraUBO {

		GPUColorSpectraDescription ColorSpectraDescription;
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

	struct GPUMaterial {
		alignas(16) glm::vec4 Albedo;
		// alignas(4) float Roughness;
		alignas(4) float Metallic;
		// alignas(16) glm::vec3 EmissionColor;
		// alignas(4) float EmissionPower;
		alignas(16) GPUColorSpectraDescription ColorSpectraDescription;
	};

	void Environment::Init(EnvironmentCreateInfo createInfo) {

		m_CreateInfo = createInfo;

		// TEMPORARY STUFF FROM RAYTRACE EXAMPLE
		m_UniformBuffer = LoFox::UniformBuffer::Create(sizeof(UBO)); // Unused at the moment
		m_CameraUniformBuffer = LoFox::UniformBuffer::Create(sizeof(GPUCameraUBO));

		m_ObjectDescriptionBuffer = LoFox::StorageBuffer::Create(1000, sizeof(GPUObjectDescription));
		m_ObjectFragmentsBuffer = LoFox::StorageBuffer::Create(1000, sizeof(GPUObjectFragment));

		m_MaterialsBuffer = LoFox::StorageBuffer::Create(1000, sizeof(GPUMaterial));

		m_SpectraBuffer = LoFox::StorageBuffer::Create(1000, sizeof(float));

		uint32_t width = LoFox::Application::GetInstance().GetActiveWindow()->GetWindowData().Width;
		uint32_t height = LoFox::Application::GetInstance().GetActiveWindow()->GetWindowData().Height;

		m_FinalImageRenderData.FinalImage = LoFox::StorageImage::Create(width, height);

		m_FinalImageRenderData.GraphicsResourceLayout = LoFox::ResourceLayout::Create({
			{ LoFox::ShaderType::Fragment, m_FinalImageRenderData.FinalImage	, true},
		});

		m_RaytraceRendererData.RaytraceResourceLayout = LoFox::ResourceLayout::Create({
			{ LoFox::ShaderType::Compute,	m_UniformBuffer	},
			{ LoFox::ShaderType::Compute,	m_CameraUniformBuffer	},
			{ LoFox::ShaderType::Compute,	m_ObjectDescriptionBuffer },
			{ LoFox::ShaderType::Compute,	m_ObjectFragmentsBuffer },
			{ LoFox::ShaderType::Compute,	m_MaterialsBuffer },
			{ LoFox::ShaderType::Compute,	m_SpectraBuffer },
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

		UpdateBuffers();
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

		UpdateBuffers();

		uint32_t width = LoFox::Application::GetInstance().GetActiveWindow()->GetWindowData().Width;
		uint32_t height = LoFox::Application::GetInstance().GetActiveWindow()->GetWindowData().Height;

		LoFox::Renderer::BeginFrame({ 1.0f, 0.0f, 1.0f });

		m_RaytraceRendererData.RaytracePipeline->Dispatch(width, height, 8, 8);

		LoFox::Renderer::SetActivePipeline(m_FinalImageRenderData.GraphicsPipeline);
		LoFox::Renderer::Draw(m_FinalImageRenderData.IndexBuffer, m_FinalImageRenderData.VertexBuffer);

		LoFox::Renderer::EndFrame();
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
		m_CameraUniformBuffer->Destroy();
		m_ObjectDescriptionBuffer->Destroy();
		m_ObjectFragmentsBuffer->Destroy();
		m_MaterialsBuffer->Destroy();
		m_SpectraBuffer->Destroy();

		// Pipelines
		m_RaytraceRendererData.RaytracePipeline->Destroy(); // All pipelines other than the graphicspipeline provided to the Renderer must be destroyed
		m_FinalImageRenderData.GraphicsPipeline->Destroy();

		// Empty the whole environment (this way any shared_ptrs should get released)
		m_Self = nullptr;
		m_Objects = {};
		m_Materials = {};
	}

	LoFox::Ref<Object> Environment::CreateObject(glm::vec3 pos, float radius, LoFox::Ref<Material> material) {

		LoFox::Ref<Object> object = LoFox::CreateRef<Object>(m_Self, pos, radius, material);
		m_Objects.push_back(object);
		return object;
	}

	LoFox::Ref<Material> Environment::CreateMaterial(glm::vec4 color, float metallic) {

		uint32_t matIndex = m_Materials.size();
		LoFox::Ref<Material> mat = LoFox::CreateRef<Material>(m_Self, matIndex, color, metallic);
		m_Materials.push_back(mat);
		return mat;
	}

	void Environment::UpdateBuffers() {

		uint32_t width = LoFox::Application::GetInstance().GetActiveWindow()->GetWindowData().Width;
		uint32_t height = LoFox::Application::GetInstance().GetActiveWindow()->GetWindowData().Height;

		// TEMPORARY STUFF FROM RAYTRACE EXAMPLE
		UBO ubo = {};
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -5.0f);
		glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f); // Forward into the screen goes into the negative z-direction.
		ubo.view = glm::lookAt(cameraPos, cameraPos + forward, glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.proj = glm::perspectiveFov(glm::radians(45.0f), (float)width, (float)height, 0.1f, 4000.0f);

		ubo.invView = glm::inverse(ubo.view);
		ubo.invProj = glm::inverse(ubo.proj);

		ubo.Time = m_SimulationTime;

		m_UniformBuffer->SetData(&ubo);
		// END OF TEMPORARY STUFF FROM RAYTRACE EXAMPLE

		std::vector<float> spectra = {};
		std::vector<GPUObjectDescription> objectDescriptions = {};
		std::vector<GPUObjectFragment> objectFragments = {};
		std::vector<GPUMaterial> materials = {};

		auto FillInAndUploadColorSpectraDescription = [&](LoFox::Ref<ColorSpectra> colorSpectra, GPUColorSpectraDescription& description) {

			description.RelativeWeights = colorSpectra->GetRelativeWeights();

			LoFox::Ref<Spectrum> infraredSpectrum = colorSpectra->GetInfraredColorSpectrum();
			description.InfraredSpectrumIndex = spectra.size();
			description.InfraredSpectrumSize =			infraredSpectrum->GetWaveLengthCount();
			description.InfraredSpectrumMinWaveLength = infraredSpectrum->GetMinWaveLength() / Units::nm;
			description.InfraredSpectrumMaxWaveLength = infraredSpectrum->GetMaxWaveLength() / Units::nm;
			for (float wl : infraredSpectrum->GetWaveLengths())
				spectra.push_back(wl);

			LoFox::Ref<Spectrum> redSpectrum = colorSpectra->GetRedColorSpectrum();
			description.RedSpectrumIndex = spectra.size();
			description.RedSpectrumSize =			redSpectrum->GetWaveLengthCount();
			description.RedSpectrumMinWaveLength =	redSpectrum->GetMinWaveLength() / Units::nm;
			description.RedSpectrumMaxWaveLength =	redSpectrum->GetMaxWaveLength() / Units::nm;
			for (float wl : redSpectrum->GetWaveLengths())
				spectra.push_back(wl);

			description.GreenSpectrumIndex = spectra.size();
			LoFox::Ref<Spectrum> greenSpectrum = colorSpectra->GetGreenColorSpectrum();
			description.GreenSpectrumSize =				greenSpectrum->GetWaveLengthCount();
			description.GreenSpectrumMinWaveLength =	greenSpectrum->GetMinWaveLength() / Units::nm;
			description.GreenSpectrumMaxWaveLength =	greenSpectrum->GetMaxWaveLength() / Units::nm;
			for (float wl : greenSpectrum->GetWaveLengths())
				spectra.push_back(wl);

			description.BlueSpectrumIndex = spectra.size();
			LoFox::Ref<Spectrum> blueSpectrum = colorSpectra->GetBlueColorSpectrum();
			description.BlueSpectrumSize =			blueSpectrum->GetWaveLengthCount();
			description.BlueSpectrumMinWaveLength =	blueSpectrum->GetMinWaveLength() / Units::nm;
			description.BlueSpectrumMaxWaveLength =	blueSpectrum->GetMaxWaveLength() / Units::nm;
			for (float wl : blueSpectrum->GetWaveLengths())
				spectra.push_back(wl);
		};

		GPUCameraUBO cameraData = {};
		FillInAndUploadColorSpectraDescription(Defaults::EyeCamera::SensorColorSpectra, cameraData.ColorSpectraDescription);
		m_CameraUniformBuffer->SetData(&cameraData);

		// Objects
		for (auto object : m_Objects) {

			GPUObjectDescription description = {};
			description.LatestFragmentIndex = objectFragments.size();

			{
				GPUObjectFragment fragment = {};
				fragment.MaterialIndex = object->m_Material->m_Name;
				fragment.Pos = object->m_Pos / Units::m; // GPU shader uses meters

				objectFragments.push_back(fragment);
			}

			description.NumberOfFragments = 1;
			description.Radius = object->m_Radius / Units::m; // GPU shader uses meters

			objectDescriptions.push_back(description);
		}

		// Materials
		for (auto mat : m_Materials) {

			GPUMaterial gpuMat = {};
			gpuMat.Albedo = mat->m_Color;
			gpuMat.Metallic = mat->m_Metallic;
			FillInAndUploadColorSpectraDescription(mat->m_ColorSpectra, gpuMat.ColorSpectraDescription);

			materials.push_back(gpuMat);
		}
		// materials.resize(3);
		// materials[0].Albedo = glm::vec3(1.0f, 1.0f, 0.0f);
		// materials[0].Roughness = 0.05f;
		// materials[0].Metallic = 0.2f;
		// materials[0].EmissionColor = glm::vec3(1.0f, 1.0f, 0.0f);
		// materials[0].EmissionPower = 0.0f;
		// 
		// materials[1].Albedo = glm::vec3(1.0f, 0.0f, 1.0f);
		// materials[1].Roughness = 0.1f;
		// materials[1].Metallic = 0.2f;
		// materials[1].EmissionColor = glm::vec3(1.0f, 0.0f, 1.0f);
		// materials[1].EmissionPower = 0.0f;
		// 
		// materials[2].Albedo = glm::vec3(0.0f, 1.0f, 1.0f);
		// materials[2].Roughness = 0.1f;
		// materials[2].Metallic = 0.2f;
		// materials[2].EmissionColor = glm::vec3(1.0f, 0.7f, 0.3f);
		// materials[2].EmissionPower = 1.0f;

		m_ObjectDescriptionBuffer->SetData(objectDescriptions.size(), objectDescriptions.data());
		m_ObjectFragmentsBuffer->SetData(objectFragments.size(), objectFragments.data());
		m_MaterialsBuffer->SetData(materials.size(), materials.data());
		m_SpectraBuffer->SetData(spectra.size(), spectra.data());
	}
}