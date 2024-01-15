#pragma once

#include <LoFox.h>

#include "RelRays/Core/Object.h"
#include "RelRays/Core/Camera.h"

#include "RelRays/Physics/Units.h"
#include "RelRays/Physics/Constants.h"

namespace RelRays {

	struct EnvironmentCreateInfo {

		uint32_t RenderTargetWidth = 2000;
		uint32_t RenderTargetHeight = 1000;

		bool UseConstantTimeStep = false; // When set to false, the timestep per OnUpdate call will depend on the time between calls. When set to true, the timestep is constant (see ConstantTimeStepValue).
		float ConstantTimeStepValue = 1.0f / 60.0f * Units::s; // When UseConstantTimeStep is enabled, this value represents the timestep for each OnUpdate call.
	
		bool ApplyDopplerShift = true;
	};

	class Environment {

	public:
		void OnUpdate();
		void RenderFrame(uint32_t viewportWidth, uint32_t viewportHeight);
		void Destroy();

		LoFox::Ref<Object> CreateObject(glm::vec3 pos, LoFox::Ref<Material> material, LoFox::Ref<Model> model);
		LoFox::Ref<Material> CreateMaterial(glm::vec4 albedo, glm::vec4 emissionColor, float emissionStrength, float absorption);
		LoFox::Ref<Model> CreateModelFromPath(const std::string& objPath);
		LoFox::Ref<Camera> CreateCamera(glm::vec3 pos, glm::vec3 viewDirection, Sensor sensor, bool makeActiveCamera = false);

		void RenderImGuiRenderSettings();
		void RenderImGuiRenderStats();

		float GetSimulationTime() { return m_SimulationTime; }
		float GetProperTime() { return m_ProperTime; }
		// uint64_t GetFinalImageImTextureID() { return *(uint64_t*)m_FinalImageRenderData.Framebuffer->GetAttachmentImTextureID(0); }
		uint64_t GetFinalImageImTextureID() { return *(uint64_t*)m_FinalImageRenderData.FinalImage->GetImTextureID(); }

		static LoFox::Ref<Environment> Create(EnvironmentCreateInfo createInfo);
	private:
		void SetSelf(LoFox::Ref<Environment> env);
		void Init(EnvironmentCreateInfo createInfo);

		void UpdateBuffers(uint32_t viewportWidth, uint32_t viewportHeight);
	private:
		LoFox::Ref<Environment> m_Self;
		EnvironmentCreateInfo m_CreateInfo;

		std::vector<LoFox::Ref<Object>> m_Objects;
		std::vector<LoFox::Ref<Material>> m_Materials;
		std::vector<LoFox::Ref<Camera>> m_Cameras;
		LoFox::Ref<Camera> m_ActiveCamera = nullptr;

		LoFox::Ref<LoFox::StorageBuffer> m_ObjectDescriptionBuffer;
		LoFox::Ref<LoFox::StorageBuffer> m_ObjectFragmentsBuffer;
		LoFox::Ref<LoFox::StorageBuffer> m_MaterialsBuffer;
		LoFox::Ref<LoFox::StorageBuffer> m_SpectraBuffer;
		LoFox::Ref<LoFox::StorageBuffer> m_VertexBuffer;
		LoFox::Ref<LoFox::StorageBuffer> m_IndexBuffer;

		LoFox::Ref<LoFox::UniformBuffer> m_CameraUniformBuffer;
		LoFox::Ref<LoFox::UniformBuffer> m_SceneUniformBuffer;
		LoFox::Ref<LoFox::UniformBuffer> m_RenderSettingsUniformBuffer;

		struct RaytraceRendererData {
			LoFox::Ref<LoFox::ResourceLayout>	GeneralResourceLayout;

			LoFox::Ref<LoFox::Shader>			SRT_Wavelengths_Shader;
			LoFox::Ref<LoFox::ComputePipeline>	SRT_Wavelengths_Pipeline;

			LoFox::Ref<LoFox::Shader>			SRT_NoDoppler_Shader;
			LoFox::Ref<LoFox::ComputePipeline>	SRT_NoDoppler_Pipeline;
		};
		RaytraceRendererData m_RaytraceRendererData = {};

		struct FinalImageRenderData {
			LoFox::Ref<LoFox::StorageImage>		FinalImage;
			LoFox::Ref<LoFox::Shader>			VertexShader;
			LoFox::Ref<LoFox::Shader>			FragmentShader;
			LoFox::Ref<LoFox::GraphicsPipeline>	GraphicsPipeline;
			LoFox::Ref<LoFox::ResourceLayout>	GraphicsResourceLayout;
			LoFox::Ref<LoFox::VertexBuffer>		VertexBuffer;
			LoFox::Ref<LoFox::IndexBuffer>		IndexBuffer;
			LoFox::Ref<LoFox::Framebuffer>		Framebuffer;

			struct QuadVertex {
				glm::vec3 Position;
				glm::vec2 TexCoord;
			};
			const std::vector<QuadVertex> vertices = {
					{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
					{{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
					{{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
					{{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
			};
			const std::vector<uint32_t> vertexIndices = { 0, 1, 2, 2, 3, 0, };

			uint32_t FinalImageWidth, FinalImageHeight;
		};
		FinalImageRenderData m_FinalImageRenderData = {};

		struct RenderSettings {
			int RayBounces = 7;
			int Samples = 1;
			bool ApplyDopplerShift = true;
			glm::vec4 AmbientLight = glm::vec4(0.10f);
		};
		RenderSettings m_RenderSettings = {};
		struct RenderStats {
			uint32_t ObjectCount = 0;
			uint32_t ObjectFragmentCount = 0;
			uint32_t VertexCount = 0;
			uint32_t IndexCount = 0;

			float RealTimeFPS = 0.0f; // represents the FPS experienced by the user
			float SimulationFPS = 0.0f; // represents the frames per simulation time seconds (FPS relative to m_SimulationTime). Handy for videos
		};
		RenderStats m_RenderStats = {};

		float m_RealTime = 0.0f; // Time that has passed in the real world (mostly used for FPS calculations)
		float m_SimulationTime = 0.0f; // Time measured by the camera (or sum of the times measured by multiple cameras)
		float m_ProperTime = 0.0f; // Time measured by the never moving origin
	};
}