#include "RelRaysDevLayer.h"
#include <ImGui/imgui.h>

namespace LoFox {

	void RelRaysDevLayer::OnAttach() {

		RelRays::EnvironmentCreateInfo envCreateInfo = {}; // Defaults are good
		// envCreateInfo.RenderTargetWidth = 320;
		// envCreateInfo.RenderTargetHeight = 160;
		// envCreateInfo.UseConstantTimeStep = true;
		envCreateInfo.ApplyDopplerShift = false;
		envCreateInfo.CustomSpeedOfLight = 100;
		m_Env = RelRays::Environment::Create(envCreateInfo);

		m_PurpleMaterial = m_Env->CreateMaterial(
			{ 1.0f, 0.1f, 1.0f, 0.0f},	// Purple albedo
			{ 0.0f, 0.0f, 0.0f, 0.0f },	// Don't make it emit light
			0.0f,						// Don't make it emit light
			0.2f						// Absorption
		);
		m_YellowMaterial = m_Env->CreateMaterial(
			{ 1.0f, 1.0f, 0.1f, 0.0f }, // Yellow albedo
			{ 1.0f, 1.0f, 0.1f, 0.0f },	// Light emission in same color as albedo
			50.0f * Units::W,			// Light strength
			0.2f						// Absorption
		);
		m_WhiteMaterial = m_Env->CreateMaterial(
			{ 1.0f, 1.0f, 1.0f, 0.0f }, // White albedo
			{ 0.0f, 0.0f, 0.0f, 0.0f },	// Don't make it emit light
			0.0f,						// Don't make it emit light
			0.2f						// Absorption
		);

		m_SpaceshipModel = m_Env->CreateModelFromPath("Assets/Models/spaceship.obj");
		m_CubeModel = m_Env->CreateModelFromPath("Assets/Models/cube.obj");

		m_TestObject1 = m_Env->CreateObject(glm::vec3(2.0f, 1.0f, -50.0f) * Units::m,	 	m_YellowMaterial,	m_CubeModel);
		m_TestObject2 = m_Env->CreateObject(glm::vec3(-3.0f, -10.0f, -50.0f) * Units::m, 	m_WhiteMaterial,	m_SpaceshipModel);
		m_TestObject3 = m_Env->CreateObject(glm::vec3(1.0f, 10.0f, -50.0f) * Units::m,	 	m_PurpleMaterial,	m_SpaceshipModel);

		m_TestObject1->SetVel(glm::vec3(1.0f, 0.0f, 0.0f) * (Units::m / Units::s));

		m_MainCamera = m_Env->CreateCamera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), RelRays::Defaults::Sensors::BlockCamera, true);
		m_SecondaryCamera = m_Env->CreateCamera(glm::vec3(0.0f, 0.0f, -100.0f), glm::vec3(0.0f, 0.0f, 1.0f), RelRays::Defaults::Sensors::EyeCamera, false);
	}
	void RelRaysDevLayer::OnDetach() {

		m_Env->Destroy();
	}

	void RelRaysDevLayer::OnUpdate(float ts) {

		m_Time += ts;
		float FPS = GetFPS(ts);
		Application::GetInstance().GetActiveWindow()->SetTitle("RelRays Dev Application: " + std::to_string(FPS) + " FPS" + " Time: " + std::to_string(m_Time) + " | Simulation Time: " + std::to_string(m_Env->GetSimulationTime()));
	
		m_Env->OnUpdate();
		if (!m_ViewportWidth == 0 && !m_ViewportHeight == 0) // Only render when viewport is non-zero (minimizing viewport, ...)
			m_Env->RenderFrame(m_ViewportWidth, m_ViewportHeight);
	}

	void RelRaysDevLayer::OnImGuiRender() {

		m_Env->RenderImGuiRenderSettings();
		m_Env->RenderImGuiRenderStats();

		{// Scene settings
			ImGui::Begin("Scene");

			static float t = 1.0f;
			if (ImGui::Button("Change course")) {
				t *= -1.0f;
				m_TestObject1->SetVel(t * glm::vec3(0.0f, 1.0f, 0.0f) * (Units::m / Units::s));
			}
			if (ImGui::Button("Reset cube")) {
				m_TestObject1->SetPos(glm::vec3(2.0f, 1.0f, -50.0f));
				m_TestObject1->SetVel(glm::vec3(0.0f, 1.0f, 0.0f));
			}
			if (ImGui::Button("Create new cube")) {
				m_Env->CreateObject({ 0.0f, -5.0f, 0.0f }, m_PurpleMaterial, m_CubeModel)->SetVel({ 0.0f, 0.0f, -5.0f }); // We don't store the ref to the object as this is just a demo
			}

			ImGui::End();
		}

		{ // Viewport
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
			ImGui::Begin("Viewport");

			ImVec2 extent = ImGui::GetContentRegionAvail();
			m_ViewportWidth = extent.x;
			m_ViewportHeight = extent.y;

			uint64_t textureID = m_Env->GetFinalImageImTextureID();
			ImGui::Image((void*)textureID, extent, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

			ImGui::PopStyleVar();

			ImGui::End();
		}
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