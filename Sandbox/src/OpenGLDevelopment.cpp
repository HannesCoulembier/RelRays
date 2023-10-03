#include "OpenGLDevelopment.h"

namespace LoFox {

	void OpenGLDevLayer::OnAttach() {

	}
	void OpenGLDevLayer::OnDetach() {

	}

	void OpenGLDevLayer::OnUpdate(float ts) {

		m_Time += ts;
		Renderer::BeginFrame({ glm::abs(glm::sin(2.0f*m_Time)), 1.0f, 0.0f });

		Renderer::EndFrame();

	}
	void OpenGLDevLayer::OnEvent(LoFox::Event& event) {

	}
}