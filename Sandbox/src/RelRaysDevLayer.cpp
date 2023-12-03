#include "RelRaysDevLayer.h"

namespace LoFox {

	void RelRaysDevLayer::OnAttach() {

		RelRays::EnvironmentCreateInfo envCreateInfo = {}; // Defaults are good
		m_Env = RelRays::Environment::Create(envCreateInfo);

		m_PurpleMaterial = m_Env->CreateMaterial({ 1.0f, 0.0f, 1.0f }, 0.2f);
		m_YellowMaterial = m_Env->CreateMaterial({ 1.0f, 1.0f, 0.0f }, 0.2f);

		m_TestObject1 = m_Env->CreateObject(glm::vec3(2.0f, 1.0f, 0.0f) * Units::m, 1.0f * Units::m, m_YellowMaterial);
		m_TestObject2 = m_Env->CreateObject(glm::vec3(0.0f, -0.135f, 0.0f) * Units::m, 1.0f * Units::m, m_YellowMaterial);
		m_TestObject3 = m_Env->CreateObject(glm::vec3(1.0f, -101.0f, -5.0f) * Units::m, 100.0f * Units::m, m_PurpleMaterial);
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