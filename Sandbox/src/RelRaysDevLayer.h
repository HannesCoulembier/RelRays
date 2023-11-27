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
		void OnEvent(LoFox::Event& event);
	private:
		float GetFPS(float ts);
	private:
		Ref<RelRays::Environment> m_Env;
		Ref<RelRays::Object> m_TestObject;
		float m_Time = 0;
	};
}