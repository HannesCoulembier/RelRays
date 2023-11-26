#include "RelRaysDevLayer.h"

namespace LoFox {

	void RelRaysDevLayer::OnAttach() {

		RelRays::EnvironmentCreateInfo envCreateInfo = {}; // Defaults are good
		m_Env = RelRays::Environment::Create(envCreateInfo);

		m_TestObject = m_Env->CreateObject();
	}
	void RelRaysDevLayer::OnDetach() {

		m_Env->Destroy();
	}

	void RelRaysDevLayer::OnUpdate(float ts) {

		m_Time += ts;
		static float avgFPS = 0;
		static uint32_t frames = 0;
		frames++;
		float FPS = 1.0f / ts;
		avgFPS += (FPS - avgFPS) / (float)frames;
		Application::GetInstance().GetActiveWindow()->SetTitle("RelRays Dev Application: " + std::to_string(FPS) + " FPS (avg: " + std::to_string(avgFPS) + ")" + " Time: " + std::to_string(m_Time) + " | Simulation Time: " + std::to_string(m_Env->GetSimulationTime()));
	
		m_Env->OnUpdate();
		m_Env->RenderFrame();
	}

	void RelRaysDevLayer::OnEvent(LoFox::Event& event) {

	}
}