#pragma once

#include <LoFox.h>

#include "Object.h"

namespace RelRays {

	class Environment {

	public:
		void OnUpdate(float ts);
		void RenderFrame();
		void Destroy();

		LoFox::Ref<Object> CreateObject();

		static LoFox::Ref<Environment> Create();
	private:
		void SetSelf(LoFox::Ref<Environment> env);
		void Init();

		// TEMPORARY STUFF FROM RAYTRACE EXAMPLE
		void UpdateUniformBuffer();
		void SetStorageBuffers();
	private:
		LoFox::Ref<Environment> m_Self;
		std::vector<LoFox::Ref<Object>> m_Objects;


		// TEMPORARY STUFF FROM RAYTRACE EXAMPLE
		LoFox::Ref<LoFox::Shader> m_ComputeShader;
		LoFox::Ref<LoFox::Shader> m_VertexShader;
		LoFox::Ref<LoFox::Shader> m_FragmentShader;

		LoFox::Ref<LoFox::ComputePipeline> m_RaytracePipeline;
		LoFox::Ref<LoFox::ResourceLayout> m_RaytraceResourceLayout;
		LoFox::Ref<LoFox::UniformBuffer> m_UniformBuffer; // Unused at the moment (for camera data)
		LoFox::Ref<LoFox::StorageImage> m_FinalImage;

		// After the image is rendered by the compute shader, we present it via a graphics pipeline
		LoFox::Ref<LoFox::GraphicsPipeline> m_GraphicsPipeline;
		LoFox::Ref<LoFox::ResourceLayout> m_GraphicsResourceLayout;
		LoFox::Ref<LoFox::VertexBuffer> m_VertexBuffer;
		LoFox::Ref<LoFox::IndexBuffer> m_IndexBuffer;

		LoFox::Ref<LoFox::StorageBuffer> m_SphereBuffer;
		LoFox::Ref<LoFox::StorageBuffer> m_MaterialBuffer;
		float m_Time = 0;
		int m_FrameIndex = 0;
	};
}