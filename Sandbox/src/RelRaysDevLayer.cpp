#include "RelRaysDevLayer.h"

namespace LoFox {

	void RelRaysDevLayer::OnAttach() {

		m_World = RelRays::World::Create();

		Ref<RelRays::Object> object = m_World->CreateObject();
	}
	void RelRaysDevLayer::OnDetach() {

		m_World->Destroy();
	}

	void RelRaysDevLayer::OnUpdate(float ts) {

		m_Time += ts;
		static float avgFPS = 0;
		static uint32_t frames = 0;
		frames++;
		float FPS = 1.0f / ts;
		avgFPS += (FPS - avgFPS) / (float)frames;
		Application::GetInstance().GetActiveWindow()->SetTitle("RelRays Dev Application: " + std::to_string(FPS) + " FPS (avg: " + std::to_string(avgFPS) + ")" + " Time: " + std::to_string(m_Time));
	
		m_World->OnUpdate(ts);
		m_World->RenderFrame();
	}

	void RelRaysDevLayer::OnEvent(LoFox::Event& event) {

	}
}