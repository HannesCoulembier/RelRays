#pragma once

#include "LoFox/Events/Event.h"

namespace LoFox {

	class Application;

	class Layer {

	public:
		virtual ~Layer() = default;

		virtual void OnAttach() {} // Equivalent to constructor
		virtual void OnDetach() {} // Equivalent to destructor

		virtual void OnUpdate(float ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}
	};
}