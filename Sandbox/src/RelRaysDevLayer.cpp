#include "RelRaysDevLayer.h"
#include <ImGui/imgui.h>

namespace LoFox {

	void RelRaysDevLayer::OnAttach() {

		RelRays::EnvironmentCreateInfo envCreateInfo = {}; // Defaults are good
		// envCreateInfo.RenderTargetWidth = 100;
		// envCreateInfo.RenderTargetHeight = 50;
		m_Env = RelRays::Environment::Create(envCreateInfo);

		m_PurpleMaterial = m_Env->CreateMaterial(
			{ 0.0f, 1.0f, 0.1f, 1.0f }, // Make the material purple
			{ 0.0f, 0.0f, 0.0f, 0.0f },	// Don't make it emit light
			0.0f,						// Don't make it emit light
			0.2f						// Metallic
		);
		m_YellowMaterial = m_Env->CreateMaterial(
			{ 0.0f, 1.0f, 1.0f, 0.1f }, // Yellow albedo
			{ 0.0f, 1.0f, 1.0f, 0.1f },	// Light emission in same color as albedo
			0.04f,						// Light strength
			0.2f						// Metallic
		);

		m_SpaceshipModel = m_Env->CreateModelFromPath("Assets/Models/spaceship.obj");
		m_SpaceshipModel = m_Env->CreateModelFromPath("Assets/Models/spaceship.obj");
		m_CubeModel = m_Env->CreateModelFromPath("Assets/Models/cube.obj");

		m_TestObject1 = m_Env->CreateObject(glm::vec3(2.0f, 1.0f, -1.0f) * Units::m, m_YellowMaterial, m_CubeModel);
		m_TestObject2 = m_Env->CreateObject(glm::vec3(-3.0f, -10.0f, -50.0f) * Units::m, m_YellowMaterial, m_SpaceshipModel);
		m_TestObject3 = m_Env->CreateObject(glm::vec3(1.0f, 10.0f, -50.0f) * Units::m, m_PurpleMaterial, m_SpaceshipModel);
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