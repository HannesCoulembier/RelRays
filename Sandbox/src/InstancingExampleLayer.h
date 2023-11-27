
// LEGACY CODE
/*
#pragma once

#include <LoFox.h>

namespace LoFox {

	class InstancingExampleLayer : public Layer {

		struct QuadVertex {

			glm::vec3 Position;
			glm::vec3 Color;
			glm::vec2 TexCoord;
			uint32_t TexIndex;
		};

		struct ObjectData {

			alignas(16) glm::mat4 model; // Still 64 bits
		};

		struct UBO {

			alignas(16) glm::mat4 view;
			alignas(16) glm::mat4 proj;
		};

		const std::vector<QuadVertex> vertices = {
			{ {-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0},
			{ {0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, 0},
			{ {0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, 0},
			{ {-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, 0},

			{ {-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}, 0},
			{ {0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}, 0},
			{ {0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}, 0},
			{ {-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, 0}
		};

		const std::vector<uint32_t> vertexIndices = {
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4,
		};

	public:
		InstancingExampleLayer() {}
		~InstancingExampleLayer() {}

		void OnAttach();
		void OnDetach();

		void OnUpdate(float ts);
		void UpdateUniformBuffer();
		void UpdateStorageBuffer();
		void OnEvent(LoFox::Event& event);
	private:
	private:
		Ref<Texture> m_Texture1;
		Ref<Texture> m_Texture2;
		Ref<TextureAtlas> m_TextureAtlas;
		Ref<UniformBuffer> m_UniformBuffer;
		Ref<StorageBuffer> m_StorageBuffer;

		Ref<GraphicsPipeline> m_GraphicsPipeline;
		Ref<ResourceLayout> m_GraphicsResourceLayout;
		Ref<VertexBuffer> m_VertexBuffer;
		Ref<IndexBuffer> m_IndexBuffer;

		float m_Time = 0;
	};
}
*/