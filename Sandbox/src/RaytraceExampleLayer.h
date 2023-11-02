#pragma once

#include <LoFox.h>

namespace LoFox {

	class RaytraceExampleLayer : public Layer {

		struct QuadVertex {

			glm::vec3 Position;
			glm::vec2 TexCoord;
		};

		// struct PushConstantObject {
		// 
		// 	alignas(4) float Time;
		// 	alignas(4) int FrameIndex;
		// };

		struct UBO {

			alignas(16) glm::mat4 view;
			alignas(16) glm::mat4 proj;
			alignas(16) glm::mat4 invView;
			alignas(16) glm::mat4 invProj;
			alignas(4) float Time;
			alignas(4) int FrameIndex;
		};

		const std::vector<QuadVertex> vertices = {
			{{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
			{{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
			{{1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
			{{-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
		};

		const std::vector<uint32_t> vertexIndices = {
			0, 1, 2, 2, 3, 0,
		};

		struct Sphere {
			alignas(16) glm::vec3 Position;
			alignas(4) float Radius;
			alignas(4) int MaterialIndex = 0;
		};

		struct Material {
			alignas(16) glm::vec3 Albedo;
			alignas(4) float Roughness;
			alignas(4) float Metallic;
			alignas(16) glm::vec3 EmissionColor;
			alignas(4) float EmissionPower;
		};

	public:
		RaytraceExampleLayer() {}
		~RaytraceExampleLayer() {}

		void OnAttach();
		void OnDetach();

		void OnUpdate(float ts);
		void UpdateUniformBuffer();
		void SetStorageBuffers();
		void OnEvent(LoFox::Event& event);
	private:
	private:
		Ref<Shader> m_ComputeShader;
		Ref<Shader> m_VertexShader;
		Ref<Shader> m_FragmentShader;

		Ref<ComputePipeline> m_RaytracePipeline;
		Ref<ResourceLayout> m_RaytraceResourceLayout;
		Ref<UniformBuffer> m_UniformBuffer; // Unused at the moment (for camera data)
		Ref<StorageImage> m_FinalImage;

		// After the image is rendered by the compute shader, we present it via a graphics pipeline
		Ref<GraphicsPipeline> m_GraphicsPipeline;
		Ref<ResourceLayout> m_GraphicsResourceLayout;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		Ref<StorageBuffer> m_SphereBuffer;
		Ref<StorageBuffer> m_MaterialBuffer;

		float m_Time = 0;
		int m_FrameIndex = 0;
	};
}