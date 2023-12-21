#pragma once

#include <LoFox.h>

#include "RelRays/Core/Object.h"

#include "RelRays/Physics/Units.h"
#include "RelRays/Physics/Constants.h"

namespace RelRays {

	struct EnvironmentCreateInfo {

		uint32_t RenderTargetWidth = 2000;
		uint32_t RenderTargetHeight = 1000;

		bool UseConstantTimeStep = false; // When set to false, the timestep per OnUpdate call will depend on the time between calls. When set to true, the timestep is constant (see ConstantTimeStepValue).
		float ConstantTimeStepValue = 1.0f / 60.0f; // When UseConstantTimeStep is enabled, this value represents the timestep for each OnUpdate call.
	};

	class Environment {

	public:
		void OnUpdate();
		void RenderFrame(uint32_t viewportWidth, uint32_t viewportHeight);
		void Destroy();

		LoFox::Ref<Object> CreateObject(glm::vec3 pos, LoFox::Ref<Material> material, LoFox::Ref<Model> model);
		LoFox::Ref<Material> CreateMaterial(glm::vec4 color, float metallic);
		LoFox::Ref<Model> CreateModelFromPath(const std::string& objPath);

		void RenderImGuiRenderSettings();

		float GetSimulationTime() { return m_SimulationTime; }
		float GetProperTime() { return m_ProperTime; }
		uint64_t GetFinalImageImTextureID() { return *(uint64_t*)m_FinalImageRenderData.Framebuffer->GetAttachmentImTextureID(0); }

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

		LoFox::Ref<LoFox::StorageBuffer> m_ObjectDescriptionBuffer;
		LoFox::Ref<LoFox::StorageBuffer> m_ObjectFragmentsBuffer;
		LoFox::Ref<LoFox::StorageBuffer> m_MaterialsBuffer;
		LoFox::Ref<LoFox::StorageBuffer> m_SpectraBuffer;
		LoFox::Ref<LoFox::StorageBuffer> m_VertexBuffer;
		LoFox::Ref<LoFox::StorageBuffer> m_IndexBuffer;

		LoFox::Ref<LoFox::UniformBuffer> m_CameraUniformBuffer;
		LoFox::Ref<LoFox::UniformBuffer> m_RenderSettingsUniformBuffer;

		// BEGIN TEMPORARY STUFF FROM RAYTRACE EXAMPLE
		LoFox::Ref<LoFox::UniformBuffer> m_UniformBuffer; // Unused at the moment (for camera data)
		// END   TEMPORARY STUFF FROM RAYTRACE EXAMPLE

		struct RaytraceRendererData {
			LoFox::Ref<LoFox::Shader>			ComputeShader;
			LoFox::Ref<LoFox::ComputePipeline>	RaytracePipeline;
			LoFox::Ref<LoFox::ResourceLayout>	RaytraceResourceLayout;
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
		};
		RenderSettings m_RenderSettings = {};

		float m_SimulationTime = 0.0f; // Time measured by the camera (or sum of the times measured by multiple cameras)
		float m_LastOnUpdateTime = 0.0f; // Used to calculate FPS
		float m_ProperTime = 0.0f; // Time measured by the never moving origin
	};
}