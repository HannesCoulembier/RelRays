#include "RelRays/Core/Environment.h"
#include <ImGui/imgui.h>

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
	// END TEMPORARY STUFF RAYTRACE EXAMPLE

	struct GPURenderSettingsUBO {

		alignas(4) int		RayBounces;
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

		alignas(16) int LatestFragmentIndex;
		alignas(4) int NumberOfFragments;
		
		alignas(4) int FirstIndexIndex;
		alignas(4) int IndexCount;
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

	struct GPUVertex {
		alignas(16) glm::vec3 Position;
	};

	void Environment::Init(EnvironmentCreateInfo createInfo) {

		m_CreateInfo = createInfo;
		m_FinalImageRenderData.FinalImageWidth = int((float)(m_CreateInfo.RenderTargetWidth)/8.0f)*8;
		m_FinalImageRenderData.FinalImageHeight = int((float)(m_CreateInfo.RenderTargetHeight)/8.0f)*8;

		// TEMPORARY STUFF FROM RAYTRACE EXAMPLE
		m_UniformBuffer = LoFox::UniformBuffer::Create(sizeof(UBO)); // Unused at the moment
		// END TEMPORARY STUFF RAYTRACE EXAMPLE
		m_CameraUniformBuffer = LoFox::UniformBuffer::Create(sizeof(GPUCameraUBO));
		m_RenderSettingsUniformBuffer = LoFox::UniformBuffer::Create(sizeof(GPURenderSettingsUBO));

		m_ObjectDescriptionBuffer = LoFox::StorageBuffer::Create(1000, sizeof(GPUObjectDescription));
		m_ObjectFragmentsBuffer = LoFox::StorageBuffer::Create(1000, sizeof(GPUObjectFragment));

		m_MaterialsBuffer = LoFox::StorageBuffer::Create(1000, sizeof(GPUMaterial));

		m_SpectraBuffer = LoFox::StorageBuffer::Create(1000, sizeof(float));

		m_VertexBuffer = LoFox::StorageBuffer::Create(1000000, sizeof(GPUVertex));
		m_IndexBuffer = LoFox::StorageBuffer::Create(1000000, sizeof(int));

		m_FinalImageRenderData.FinalImage = LoFox::StorageImage::Create(m_FinalImageRenderData.FinalImageWidth, m_FinalImageRenderData.FinalImageHeight);

		LoFox::FramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.Width = m_FinalImageRenderData.FinalImageWidth;
		framebufferCreateInfo.Height = m_FinalImageRenderData.FinalImageHeight;
		framebufferCreateInfo.SwapChainTarget = false;
		framebufferCreateInfo.Attachments = {

			LoFox::FramebufferTextureFormat::RGBA8,
		};
		m_FinalImageRenderData.Framebuffer = LoFox::Framebuffer::Create(framebufferCreateInfo);

		m_FinalImageRenderData.GraphicsResourceLayout = LoFox::ResourceLayout::Create({
			{ LoFox::ShaderType::Fragment, m_FinalImageRenderData.FinalImage	, true},
		});

		m_RaytraceRendererData.RaytraceResourceLayout = LoFox::ResourceLayout::Create({
			{ LoFox::ShaderType::Compute,	m_FinalImageRenderData.FinalImage , false}, // NOTE: OpenGL only supports image bindings up until binding 7, so make them the first bindings in the layout
			{ LoFox::ShaderType::Compute,	m_UniformBuffer	},
			{ LoFox::ShaderType::Compute,	m_RenderSettingsUniformBuffer },
			{ LoFox::ShaderType::Compute,	m_CameraUniformBuffer },
			{ LoFox::ShaderType::Compute,	m_ObjectDescriptionBuffer },
			{ LoFox::ShaderType::Compute,	m_ObjectFragmentsBuffer },
			{ LoFox::ShaderType::Compute,	m_MaterialsBuffer },
			{ LoFox::ShaderType::Compute,	m_SpectraBuffer },
			{ LoFox::ShaderType::Compute,	m_VertexBuffer },
			{ LoFox::ShaderType::Compute,	m_IndexBuffer },
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

		UpdateBuffers(m_FinalImageRenderData.FinalImageWidth, m_FinalImageRenderData.FinalImageHeight);
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

	void Environment::RenderFrame(uint32_t viewportWidth, uint32_t viewportHeight) {

		UpdateBuffers(viewportWidth, viewportHeight);

		uint32_t width = m_FinalImageRenderData.Framebuffer->GetWidth();
		uint32_t height = m_FinalImageRenderData.Framebuffer->GetHeight();
		m_RaytraceRendererData.RaytracePipeline->Dispatch(width, height, 8, 8);

		LoFox::Renderer::BeginFramebuffer(m_FinalImageRenderData.Framebuffer, { 1.0f, 0.0f, 1.0f });

		LoFox::Renderer::SetActivePipeline(m_FinalImageRenderData.GraphicsPipeline);
		LoFox::Renderer::Draw(m_FinalImageRenderData.IndexBuffer, m_FinalImageRenderData.VertexBuffer);

		LoFox::Renderer::EndFramebuffer();
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
		m_RenderSettingsUniformBuffer->Destroy();

		m_ObjectDescriptionBuffer->Destroy();
		m_ObjectFragmentsBuffer->Destroy();
		m_MaterialsBuffer->Destroy();
		m_SpectraBuffer->Destroy();
		m_VertexBuffer->Destroy();
		m_IndexBuffer->Destroy();

		// Pipelines
		m_RaytraceRendererData.RaytracePipeline->Destroy(); // All pipelines other than the graphicspipeline provided to the Renderer must be destroyed
		m_FinalImageRenderData.GraphicsPipeline->Destroy();

		// Framebuffers
		m_FinalImageRenderData.Framebuffer->Destroy();

		// Empty the whole environment (this way any shared_ptrs should get released)
		m_Self = nullptr;
		m_Objects = {};
		m_Materials = {};
	}

	LoFox::Ref<Object> Environment::CreateObject(glm::vec3 pos, LoFox::Ref<Material> material, LoFox::Ref<Model> model) {

		LoFox::Ref<Object> object = LoFox::CreateRef<Object>(m_Self, pos, material, model);
		m_Objects.push_back(object);
		return object;
	}

	LoFox::Ref<Material> Environment::CreateMaterial(glm::vec4 color, float metallic) {

		uint32_t matIndex = m_Materials.size();
		LoFox::Ref<Material> mat = LoFox::CreateRef<Material>(m_Self, matIndex, color, metallic);
		m_Materials.push_back(mat);
		return mat;
	}

	LoFox::Ref<Model> Environment::CreateModelFromPath(const std::string& objPath) {

		return LoFox::CreateRef<Model>(objPath);
	}

	void Environment::UpdateBuffers(uint32_t viewportWidth, uint32_t viewportHeight) {

		uint32_t width = viewportWidth;
		uint32_t height = viewportHeight;

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
		std::vector<GPUVertex> vertices = {};
		std::vector<int> indices = {};
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

		// Uniform buffers
		GPUCameraUBO cameraData = {};
		FillInAndUploadColorSpectraDescription(Defaults::EyeCamera::SensorColorSpectra, cameraData.ColorSpectraDescription);
		m_CameraUniformBuffer->SetData(&cameraData);

		GPURenderSettingsUBO renderSettingsData = {};
		renderSettingsData.RayBounces = m_RenderSettings.RayBounces;
		m_RenderSettingsUniformBuffer->SetData(&renderSettingsData);

		// Objects
		for (auto object : m_Objects) {

			GPUObjectDescription description = {};

			// Fragments
			description.LatestFragmentIndex = objectFragments.size();
			{
				GPUObjectFragment fragment = {};
				fragment.MaterialIndex = object->m_Material->m_Name;
				fragment.Pos = object->m_Pos / Units::m; // GPU shader uses meters

				objectFragments.push_back(fragment);
			}
			description.NumberOfFragments = 1;

			// Mesh
			description.FirstIndexIndex = indices.size();
			description.IndexCount = object->m_Model->m_Indices.size();

			uint32_t firstVertexIndex = vertices.size();
			{
				for (uint32_t index : object->m_Model->m_Indices) {
					indices.push_back((int)(index + firstVertexIndex));
				}
			}
			{
				for (ModelVertex mv : object->m_Model->m_Vertices) {
					GPUVertex vertex = {};
					vertex.Position = mv.Position / Units::m; // GPU shader uses meters

					vertices.push_back(vertex);
				}
			}

			objectDescriptions.push_back(description);
		}

		// Materials
		for (auto mat : m_Materials) { // The materials are ordened by their 'names'. These are just their indices in the m_Materials vector

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
		m_VertexBuffer->SetData(vertices.size(), vertices.data());
		m_IndexBuffer->SetData(indices.size(), indices.data());
	}

	void Environment::RenderImGuiRenderSettings() {

		ImGui::Begin("Settings");

		ImGui::SliderInt("Bounces", &m_RenderSettings.RayBounces, 1, 10);
		ImGui::Separator();

		ImGui::End();
		
	}
}