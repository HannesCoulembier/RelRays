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
	private:
		Ref<RelRays::World> m_World;
		Ref<RelRays::Object> m_TestObject;
		float m_Time = 0;
	};
}