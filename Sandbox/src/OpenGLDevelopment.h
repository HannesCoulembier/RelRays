#pragma once

#include <LoFox.h>

namespace LoFox {

	struct QuadVertex {

		glm::vec3 Position;
		glm::vec3 Color;
		glm::vec2 UV;
	};

	const std::vector<QuadVertex> vertices = {
		{{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
		{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
		{{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
		{{1.0f, 1.0f, 0.0f}, {1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	};

	const std::vector<uint32_t> vertexIndices = {
		0, 1, 2, 2, 3, 0,
	};

	struct UBO {

		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
		alignas(16) glm::mat4 invView;
		alignas(16) glm::mat4 invProj;
	};

	class OpenGLDevLayer : public Layer {

	public:
		OpenGLDevLayer() {}
		~OpenGLDevLayer() {}

		void OnAttach();
		void OnDetach();

		void OnUpdate(float ts);
		void OnEvent(LoFox::Event& event);
	private:
	private:
		float m_Time = 0;
		Ref<Shader> m_VertexShader;
		Ref<Shader> m_FragmentShader;
		Ref<Shader> m_ComputeShader;
		Ref<GraphicsPipeline> m_GraphicsPipeline;
		Ref<ComputePipeline> m_ComputePipeline;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		Ref<UniformBuffer> m_CameraData;
		Ref<StorageBuffer> m_ObjectTransforms;
		Ref<Texture> m_RickTexture;
		Ref<Texture> m_PolandTexture;
		// Ref<TextureAtlas> m_TextureAtlas;
		Ref<StorageImage> m_TestStorageImage;

		Ref<Framebuffer> m_Framebuffer;

		Ref<ResourceLayout> m_ResourceLayout;
		Ref<ResourceLayout> m_ComputeResourceLayout;
	};
}