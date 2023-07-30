#pragma once

#include <LoFox.h>

namespace LoFox {

	class RaytraceExampleLayer : public Layer {

		struct QuadVertex {

			glm::vec3 Position;
			glm::vec2 TexCoord;
		};

		struct UBO { // Unused at the moment

			alignas(16) glm::mat4 view;
			alignas(16) glm::mat4 proj;
			alignas(16) glm::mat4 invView;
			alignas(16) glm::mat4 invProj;
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

	public:
		RaytraceExampleLayer() {}
		~RaytraceExampleLayer() {}

		void OnAttach();
		void OnDetach();

		void OnUpdate(float ts);
		void UpdateUniformBuffer();
		void OnEvent(LoFox::Event& event);
	private:
	private:
		Ref<ComputePipeline> m_RaytracePipeline;
		Ref<ResourceLayout> m_RaytraceResourceLayout;
		Ref<UniformBuffer> m_UniformBuffer; // Unused at the moment (for camera data)
		Ref<StorageImage> m_FinalImage;

		// After the image is rendered by the compute shader, we present it via a graphics pipeline
		Ref<GraphicsPipeline> m_GraphicsPipeline;
		Ref<ResourceLayout> m_GraphicsResourceLayout;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		float m_Time = 0;
	};
}