#pragma once

#include <LoFox.h>

namespace LoFox {

	struct QuadVertex {

		glm::vec3 Position;
		glm::vec3 Color;
	};

	const std::vector<QuadVertex> vertices = {
			{{1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{-1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
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
		Ref<GraphicsPipeline> m_Pipeline;
		Ref<VertexBuffer> m_VertexBuffer;
	};
}