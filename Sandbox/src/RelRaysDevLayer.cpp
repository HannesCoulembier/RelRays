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
		float FPS = GetFPS(ts);
		Application::GetInstance().GetActiveWindow()->SetTitle("RelRays Dev Application: " + std::to_string(FPS) + " FPS" + " Time: " + std::to_string(m_Time) + " | Simulation Time: " + std::to_string(m_Env->GetSimulationTime()));
	
		m_Env->OnUpdate();
		m_Env->RenderFrame();
	}

	void RelRaysDevLayer::OnEvent(LoFox::Event& event) {

	}

	float RelRaysDevLayer::GetFPS(float ts) {

		static float batchTime = 0.0f;
		static uint32_t batchFrames = 0;
		batchFrames++;
		batchTime += ts;
		static float FPS = batchFrames / batchTime;
		if (batchTime >= 0.5f) {
			FPS = batchFrames / batchTime;
			batchTime = 0.0f;
			batchFrames = 0;
		}
		return FPS;
	}
}