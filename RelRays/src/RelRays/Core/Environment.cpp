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
	};
	// END TEMPORARY STUFF RAYTRACE EXAMPLE

	struct GPURenderSettingsUBO {

		alignas(4) int		RayBounces;
		alignas(4) int		Samples;
		alignas(4) int		AccBuffersSampleCount;
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

	struct GPUSceneUBO {

		alignas(16) glm::vec4 AmbientLight;
		alignas(4) float OriginTime;
		alignas(4) float c;
	};

	struct GPUCameraUBO {

		alignas(16) glm::vec3 Pos;
		alignas(16) glm::vec3 Vel;
		alignas(16) glm::vec3 Acc;
		alignas(16) glm::mat4 InvView;
		alignas(16) glm::mat4 InvProj;
		alignas(16) GPUColorSpectraDescription ColorSpectraDescription;
	};

	struct GPUObjectDescription {

		alignas(16) int OldestFragmentIndex;
		alignas(4) int NumberOfFragments;
		
		alignas(4) int FirstIndexIndex;
		alignas(4) int IndexCount;
	};

	struct GPUObjectFragment {

		alignas(16) glm::vec4 StartPos; // To be extended
		alignas(16) glm::vec3 StartVel;

		alignas(4) int MaterialIndex;
	};

	struct GPUMaterial {
		alignas(16) glm::vec4 Albedo;
		// alignas(4) float Roughness;
		alignas(4) float Absorption;
		alignas(16) glm::vec4 EmissionColor;
		alignas(4) float EmissionStrength;
		alignas(16) GPUColorSpectraDescription AbsorptionColorSpectraDescription;
		alignas(16) GPUColorSpectraDescription EmissionColorSpectraDescription;
	};

	struct GPUVertex {
		alignas(16) glm::vec3 Position;
	};

	void Environment::Init(EnvironmentCreateInfo createInfo) {

		Defaults::Init();

		m_CreateInfo = createInfo;
		m_FinalImageRenderData.FinalImageWidth = int((float)(m_CreateInfo.RenderTargetWidth)/8.0f)*8;
		m_FinalImageRenderData.FinalImageHeight = int((float)(m_CreateInfo.RenderTargetHeight)/8.0f)*8;

		m_RenderSettings.ApplyDopplerShift = m_CreateInfo.ApplyDopplerShift;
		m_RenderSettings.c = m_CreateInfo.CustomSpeedOfLight;
		m_RenderSettings.cGUI = m_RenderSettings.c / (Units::m / Units::s);

		m_CameraUniformBuffer = LoFox::UniformBuffer::Create(sizeof(GPUCameraUBO));
		m_SceneUniformBuffer = LoFox::UniformBuffer::Create(sizeof(GPUSceneUBO));
		m_RenderSettingsUniformBuffer = LoFox::UniformBuffer::Create(sizeof(GPURenderSettingsUBO));

		m_ObjectDescriptionBuffer = LoFox::StorageBuffer::Create(1000, sizeof(GPUObjectDescription));
		m_ObjectFragmentsBuffer = LoFox::StorageBuffer::Create(1000, sizeof(GPUObjectFragment));

		m_MaterialsBuffer = LoFox::StorageBuffer::Create(1000, sizeof(GPUMaterial));

		m_SpectraBuffer = LoFox::StorageBuffer::Create(100000, sizeof(float));

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

		// Create Compute Shaders
		m_RaytraceRendererData.SRT_Wavelengths_Shader = LoFox::Shader::Create(LoFox::ShaderType::Compute, "Assets/Shaders/SRT_Wavelengths.comp");
		m_RaytraceRendererData.SRT_NoDoppler_Shader = LoFox::Shader::Create(LoFox::ShaderType::Compute, "Assets/Shaders/SRT_NoDoppler.comp");

		// Create Compute Pipelines
		m_RaytraceRendererData.GeneralResourceLayout = LoFox::ResourceLayout::Create({
			{ LoFox::ShaderType::Compute,	m_FinalImageRenderData.FinalImage , false}, // NOTE: OpenGL only supports image bindings up until binding 7, so make them the first bindings in the layout
			{ LoFox::ShaderType::Compute,	m_RenderSettingsUniformBuffer },
			{ LoFox::ShaderType::Compute,	m_CameraUniformBuffer },
			{ LoFox::ShaderType::Compute,	m_SceneUniformBuffer },
			{ LoFox::ShaderType::Compute,	m_ObjectDescriptionBuffer },
			{ LoFox::ShaderType::Compute,	m_ObjectFragmentsBuffer },
			{ LoFox::ShaderType::Compute,	m_MaterialsBuffer },
			{ LoFox::ShaderType::Compute,	m_SpectraBuffer },
			{ LoFox::ShaderType::Compute,	m_VertexBuffer },
			{ LoFox::ShaderType::Compute,	m_IndexBuffer },
		});

		LoFox::ComputePipelineCreateInfo computePipelineCreateInfo = {};
		computePipelineCreateInfo.ResourceLayout = m_RaytraceRendererData.GeneralResourceLayout;

		computePipelineCreateInfo.ComputeShader = m_RaytraceRendererData.SRT_Wavelengths_Shader;
		m_RaytraceRendererData.SRT_Wavelengths_Pipeline = LoFox::ComputePipeline::Create(computePipelineCreateInfo);

		computePipelineCreateInfo.ComputeShader = m_RaytraceRendererData.SRT_NoDoppler_Shader;
		m_RaytraceRendererData.SRT_NoDoppler_Pipeline = LoFox::ComputePipeline::Create(computePipelineCreateInfo);

		// Create Vertex/Indexbuffers
		// LoFox::VertexLayout vertexLayout = { // Must match QuadVertex
		// 	{ LoFox::VertexAttributeType::Float3 }, // position
		// 	{ LoFox::VertexAttributeType::Float2 }, // texCoord
		// };
		// uint32_t vertexBufferSize = sizeof(m_FinalImageRenderData.vertices[0]) * m_FinalImageRenderData.vertices.size();
		// m_FinalImageRenderData.VertexBuffer = LoFox::VertexBuffer::Create(vertexBufferSize, m_FinalImageRenderData.vertices.data(), vertexLayout);
		// m_FinalImageRenderData.IndexBuffer = LoFox::IndexBuffer::Create(m_FinalImageRenderData.vertexIndices.size(), m_FinalImageRenderData.vertexIndices.data());

		// Create Vertex/Fragment Shaders
		// m_FinalImageRenderData.FragmentShader = LoFox::Shader::Create(LoFox::ShaderType::Fragment, "Assets/Shaders/MainFragmentShader.frag");
		// m_FinalImageRenderData.VertexShader = LoFox::Shader::Create(LoFox::ShaderType::Vertex, "Assets/Shaders/MainVertexShader.vert");

		// Create Graphics pipeline
		// m_FinalImageRenderData.GraphicsResourceLayout = LoFox::ResourceLayout::Create({
		// 	{ LoFox::ShaderType::Fragment, m_FinalImageRenderData.FinalImage	, true},
		// });
		// LoFox::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
		// graphicsPipelineCreateInfo.VertexShader = m_FinalImageRenderData.VertexShader;
		// graphicsPipelineCreateInfo.FragmentShader = m_FinalImageRenderData.FragmentShader;
		// graphicsPipelineCreateInfo.ResourceLayout = m_FinalImageRenderData.GraphicsResourceLayout;
		// graphicsPipelineCreateInfo.Topology = LoFox::Topology::Triangle;
		// graphicsPipelineCreateInfo.VertexLayout = vertexLayout;
		// m_FinalImageRenderData.GraphicsPipeline = LoFox::GraphicsPipeline::Create(graphicsPipelineCreateInfo);
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

		float time = LoFox::Time::GetTime();
		float ts = time - (m_RealTime / Units::s);
		m_RenderStats.RealTimeFPS = 1 / ts;
		m_RealTime = time * Units::s;

		if (m_CreateInfo.UseConstantTimeStep) {

			m_RenderStats.SimulationFPS = 1 / (m_CreateInfo.ConstantTimeStepValue / Units::s);
			m_SimulationTime += m_CreateInfo.ConstantTimeStepValue;
		}
		else {
			m_RenderStats.SimulationFPS = m_RenderStats.RealTimeFPS;
			m_SimulationTime = m_RealTime;
		}

		m_ProperTime = m_SimulationTime; // TODO: update proper time properly (multiple modes, ...)

		for (auto camera : m_Cameras)
			camera->OnUpdate();
	}

	void Environment::RenderFrame(uint32_t viewportWidth, uint32_t viewportHeight) {

		LF_CORE_ASSERT(m_ActiveCamera, "No active camera selected for rendering!");

		UpdateBuffers(viewportWidth, viewportHeight);

		uint32_t width = m_FinalImageRenderData.Framebuffer->GetWidth();
		uint32_t height = m_FinalImageRenderData.Framebuffer->GetHeight();

		if (m_RenderSettings.ApplyDopplerShift)
			m_RaytraceRendererData.SRT_Wavelengths_Pipeline->Dispatch(width, height, 8, 8);
		else
			m_RaytraceRendererData.SRT_NoDoppler_Pipeline->Dispatch(width, height, 8, 8);

		// LoFox::Renderer::BeginFramebuffer(m_FinalImageRenderData.Framebuffer, { 1.0f, 0.0f, 1.0f });
		// 
		// LoFox::Renderer::SetActivePipeline(m_FinalImageRenderData.GraphicsPipeline);
		// LoFox::Renderer::Draw(m_FinalImageRenderData.IndexBuffer, m_FinalImageRenderData.VertexBuffer);
		// 
		// LoFox::Renderer::EndFramebuffer();
	}

	void Environment::Destroy() {

		// Shaders
		// m_FinalImageRenderData.VertexShader->Destroy();
		// m_FinalImageRenderData.FragmentShader->Destroy();
		m_RaytraceRendererData.SRT_Wavelengths_Shader->Destroy();
		m_RaytraceRendererData.SRT_NoDoppler_Shader->Destroy();

		// Render stuff
		// m_FinalImageRenderData.VertexBuffer->Destroy();
		// m_FinalImageRenderData.IndexBuffer->Destroy();
		// m_FinalImageRenderData.GraphicsResourceLayout->Destroy();
		m_RaytraceRendererData.GeneralResourceLayout->Destroy();
		m_FinalImageRenderData.FinalImage->Destroy();

		// Buffers
		m_CameraUniformBuffer->Destroy();
		m_SceneUniformBuffer->Destroy();
		m_RenderSettingsUniformBuffer->Destroy();

		m_ObjectDescriptionBuffer->Destroy();
		m_ObjectFragmentsBuffer->Destroy();
		m_MaterialsBuffer->Destroy();
		m_SpectraBuffer->Destroy();
		m_VertexBuffer->Destroy();
		m_IndexBuffer->Destroy();

		// Pipelines
		m_RaytraceRendererData.SRT_Wavelengths_Pipeline->Destroy();
		m_RaytraceRendererData.SRT_NoDoppler_Pipeline->Destroy();
		// m_FinalImageRenderData.GraphicsPipeline->Destroy();

		// Framebuffers
		m_FinalImageRenderData.Framebuffer->Destroy();

		// Empty the whole environment (this way any shared_ptrs should get released)
		m_Self = nullptr;
		m_Objects = {};
		m_Materials = {};
		m_Cameras = {};
		m_ActiveCamera = nullptr;
	}

	LoFox::Ref<Object> Environment::CreateObject(glm::vec3 pos, LoFox::Ref<Material> material, LoFox::Ref<Model> model) {

		LoFox::Ref<Object> object = LoFox::CreateRef<Object>(m_Self, pos, material, model);
		m_Objects.push_back(object);
		return object;
	}

	LoFox::Ref<Material> Environment::CreateMaterial(glm::vec4 albedo, glm::vec4 emissionColor, float emissionStrength, float absorption) {

		uint32_t matIndex = m_Materials.size();
		LoFox::Ref<Material> mat = LoFox::CreateRef<Material>(m_Self, matIndex, albedo, emissionColor, emissionStrength, absorption);
		m_Materials.push_back(mat);
		return mat;
	}

	LoFox::Ref<Model> Environment::CreateModelFromPath(const std::string& objPath) {

		return LoFox::CreateRef<Model>(objPath);
	}

	LoFox::Ref<Camera> Environment::CreateCamera(glm::vec3 pos, glm::vec3 viewDirection, Sensor sensor, bool makeActiveCamera) {

		LoFox::Ref<Camera> camera = LoFox::CreateRef<Camera>(m_Self, pos, viewDirection, sensor);
		m_Cameras.push_back(camera);
		if (makeActiveCamera)
			m_ActiveCamera = camera;
		return camera;
	}

	void Environment::UpdateBuffers(uint32_t viewportWidth, uint32_t viewportHeight) {

		GPUCameraUBO cameraData = {};
		GPUSceneUBO sceneData = {};
		GPURenderSettingsUBO renderSettingsData = {};
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
		cameraData.Pos = m_ActiveCamera->m_Pos;
		cameraData.Vel = m_ActiveCamera->m_Vel;
		cameraData.Acc = m_ActiveCamera->m_Acc;
		cameraData.InvView = glm::inverse(glm::lookAt(m_ActiveCamera->m_Pos, m_ActiveCamera->m_Pos + m_ActiveCamera->m_ViewDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
		cameraData.InvProj = glm::inverse(glm::perspectiveFov(glm::radians(45.0f), (float)viewportWidth, (float)viewportHeight, 0.1f, 4000.0f));
		FillInAndUploadColorSpectraDescription(m_ActiveCamera->m_Sensor.ColorSpectra, cameraData.ColorSpectraDescription);

		sceneData.AmbientLight = m_RenderSettings.AmbientLight * (m_RenderSettings.AmbientLightStrength / Units::W) / (m_RenderSettings.FullRGBPowerRatio / Units::W); // RGB doesn't work with watts: let 200 W equal full brightness
		// sceneData.OriginTime = m_ProperTime / Units::s;
		sceneData.OriginTime = m_RenderSettings.Time;
		sceneData.c = m_RenderSettings.c / (Units::m / Units::s);

		renderSettingsData.RayBounces = m_RenderSettings.RayBounces;
		renderSettingsData.Samples = m_RenderSettings.Samples;
		renderSettingsData.AccBuffersSampleCount = m_RenderSettings.AccBuffersSampleCount;

		// Storage buffers
		// Objects
		for (auto object : m_Objects) {

			GPUObjectDescription description = {};

			// Fragments
			description.NumberOfFragments = object->m_Fragments.size();
			description.OldestFragmentIndex = objectFragments.size();
			{
				GPUObjectFragment GPUfragment = {};
				for (auto fragment : object->m_Fragments) {

					GPUfragment.MaterialIndex = fragment.m_Material->m_Name;
					GPUfragment.StartPos = fragment.m_StartPos / glm::vec4(Units::m, Units::m, Units::m, Units::s); // GPU shader uses meters and seconds
					GPUfragment.StartVel = fragment.m_StartVel / (Units::m / Units::s); // GPU shader uses meter/second for velocity
					
					objectFragments.push_back(GPUfragment);
				}
			}

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
			gpuMat.Albedo = mat->m_Albedo;
			gpuMat.EmissionColor = mat->m_EmissionColor;
			gpuMat.EmissionStrength = (mat->m_EmissionStrength / Units::W) / (m_RenderSettings.FullRGBPowerRatio / Units::W);
			gpuMat.Absorption = mat->m_Absorption;
			FillInAndUploadColorSpectraDescription(mat->m_AbsorptionColorSpectra, gpuMat.AbsorptionColorSpectraDescription);
			FillInAndUploadColorSpectraDescription(mat->m_EmissonColorSpectra, gpuMat.EmissionColorSpectraDescription);

			materials.push_back(gpuMat);
		}

		m_CameraUniformBuffer->SetData(&cameraData);
		m_SceneUniformBuffer->SetData(&sceneData);
		m_RenderSettingsUniformBuffer->SetData(&renderSettingsData);

		m_ObjectDescriptionBuffer->SetData(objectDescriptions.size(), objectDescriptions.data());
		m_ObjectFragmentsBuffer->SetData(objectFragments.size(), objectFragments.data());
		m_MaterialsBuffer->SetData(materials.size(), materials.data());
		m_SpectraBuffer->SetData(spectra.size(), spectra.data());
		m_VertexBuffer->SetData(vertices.size(), vertices.data());
		m_IndexBuffer->SetData(indices.size(), indices.data());

		// Render stats
		m_RenderStats.ObjectCount = objectDescriptions.size();
		m_RenderStats.ObjectFragmentCount = objectFragments.size();
		m_RenderStats.VertexCount = vertices.size();
		m_RenderStats.IndexCount = indices.size();
	}

	void Environment::RenderImGuiRenderSettings() {

		ImGui::Begin("Render settings");

		ImGui::SliderInt("Bounces", &m_RenderSettings.RayBounces, 0, 10);
		ImGui::SliderInt("Samples", &m_RenderSettings.Samples, 0, 10);
		ImGui::SliderInt("Acc Buffers Samples", &m_RenderSettings.AccBuffersSampleCount, 0, 1024); // 1024 is the maximum size of the buffer in the shader. To increase this, change the maxAccBuffersSampleCount.

		ImGui::Separator();

		ImGui::Checkbox("Apply doppler shift", &m_RenderSettings.ApplyDopplerShift);

		ImGui::ColorEdit3("Ambient Lighting", &m_RenderSettings.AmbientLight.r);
		m_RenderSettings.AmbientLight.a = 0.0f;
		ImGui::SliderFloat("Ambient Lighting Strength", &m_RenderSettings.AmbientLightStrengthGUI, 0.0f, 300.0f);
		m_RenderSettings.AmbientLightStrength = m_RenderSettings.AmbientLightStrengthGUI * Units::W;
		ImGui::Separator();

		ImGui::SliderFloat3("Active camera pos", &m_ActiveCamera->m_Pos.x, -10.0f, 10.0f);

		ImGui::SliderFloat("Speed of light", &m_RenderSettings.cGUI, 0, 100);
		m_RenderSettings.c = m_RenderSettings.cGUI * (Units::m / Units::s);


		ImGui::Begin("test");
		ImGui::SliderFloat("time", &m_RenderSettings.Time, 0, 100);
		ImGui::End();

		ImGui::End();

	}

	void Environment::RenderImGuiRenderStats() {

		{ // Render stats
			ImGui::Begin("Render stats");

			ImGui::Text("Objects drawn: %d", m_RenderStats.ObjectCount);
			ImGui::Text("Object fragments drawn: %d", m_RenderStats.ObjectFragmentCount);

			ImGui::Separator();

			ImGui::Text("Vertices drawn: %d", m_RenderStats.VertexCount);
			ImGui::Text("Indices drawn:	%d", m_RenderStats.IndexCount);

			ImGui::Separator();

			ImGui::Text("Image resolution: %dx%d", m_FinalImageRenderData.FinalImageWidth, m_FinalImageRenderData.FinalImageHeight);

			ImGui::End();
		}

		{ // Simulation stats
			ImGui::Begin("Simulation stats");


			ImGui::Text("True FPS: %f", m_RenderStats.RealTimeFPS);
			ImGui::Text("Simulation FPS: %f", m_RenderStats.SimulationFPS);

			ImGui::Separator();

			ImGui::Text("Real time: %f", m_RealTime);
			ImGui::Text("Simulation time: %f", m_SimulationTime);
			ImGui::Text("Origin/proper time: %f", m_ProperTime);

			ImGui::End();
		}
	}
}