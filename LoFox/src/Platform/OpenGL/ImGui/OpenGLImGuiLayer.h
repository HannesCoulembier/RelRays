#pragma once

#include "LoFox/ImGui/ImGuiLayer.h"

#include "LoFox/Events/ApplicationEvent.h"
#include "LoFox/Events/KeyEvent.h"
#include "LoFox/Events/MouseEvent.h"

namespace LoFox {

	class OpenGLImGuiLayer : public ImGuiLayer {

	public:
		OpenGLImGuiLayer() {}
		~OpenGLImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		virtual void Begin() override;
		virtual void End() override;
	private:
	};
}
