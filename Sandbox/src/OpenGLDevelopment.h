#pragma once

#include <LoFox.h>

namespace LoFox {

	class OpenGLDevLayer : public Layer {

	public:
		OpenGLDevLayer() {}
		~OpenGLDevLayer() {}

		void OnAttach();
		void OnDetach();

		void OnUpdate(float ts);
		void OnEvent(LoFox::Event& event);
	private:
	private:
		float m_Time = 0;
	};
}