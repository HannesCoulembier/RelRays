#pragma once

#include <LoFox.h>
#include <RelRays.h>

namespace LoFox {

	class RelRaysDevLayer : public Layer {

	public:
		RelRaysDevLayer() {}
		~RelRaysDevLayer() {}

		void OnAttach();
		void OnDetach();

		void OnUpdate(float ts);
		void OnImGuiRender();
		void OnEvent(LoFox::Event& event);
	private:
		float GetFPS(float ts);
	private:
		Ref<RelRays::Environment> m_Env;
		Ref<RelRays::Material> m_PurpleMaterial, m_YellowMaterial, m_WhiteMaterial;
		Ref<RelRays::Object> m_TestObject1, m_TestObject2, m_TestObject3;
		Ref<RelRays::Model> m_SpaceshipModel, m_CubeModel;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
		float m_Time = 0;
	};
}