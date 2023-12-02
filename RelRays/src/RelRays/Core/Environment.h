#pragma once

#include <LoFox.h>

#include "RelRays/Core/Object.h"

#include "RelRays/Physics/Units.h"
#include "RelRays/Physics/Constants.h"

namespace RelRays {

	struct EnvironmentCreateInfo {
		bool UseConstantTimeStep = false; // When set to false, the timestep per OnUpdate call will depend on the time between calls. When set to true, the timestep is constant (see ConstantTimeStepValue).
		float ConstantTimeStepValue = 1.0f / 60.0f; // When UseConstantTimeStep is enabled, this value represents the timestep for each OnUpdate call.
	};

	class Environment {

	public:
		void OnUpdate();
		void RenderFrame();
		void Destroy();

		LoFox::Ref<Object> CreateObject(glm::vec3 pos, float radius);

		float GetSimulationTime() { return m_SimulationTime; }
		float GetProperTime() { return m_ProperTime; }

		static LoFox::Ref<Environment> Create(EnvironmentCreateInfo createInfo);
	private:
		void SetSelf(LoFox::Ref<Environment> env);
		void Init(EnvironmentCreateInfo createInfo);

		// TEMPORARY STUFF FROM RAYTRACE EXAMPLE
		void UpdateUniformBuffer();
		void SetStorageBuffers();
	private:
		LoFox::Ref<Environment> m_Self;
		EnvironmentCreateInfo m_CreateInfo;

		std::vector<LoFox::Ref<Object>> m_Objects;

		LoFox::Ref<LoFox::StorageBuffer> m_ObjectDescriptionBuffer;
		LoFox::Ref<LoFox::StorageBuffer> m_ObjectFragmentsBuffer;


		// TEMPORARY STUFF FROM RAYTRACE EXAMPLE
		LoFox::Ref<LoFox::UniformBuffer> m_UniformBuffer; // Unused at the moment (for camera data)

		LoFox::Ref<LoFox::StorageBuffer> m_MaterialBuffer;
		int m_FrameIndex = 0;

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
		};
		FinalImageRenderData m_FinalImageRenderData = {};

		float m_SimulationTime = 0.0f; // Time measured by the camera (or sum of the times measured by multiple cameras)
		float m_LastOnUpdateTime = 0.0f; // Used to calculate FPS
		float m_ProperTime = 0.0f; // Time measured by the never moving origin
	};
}