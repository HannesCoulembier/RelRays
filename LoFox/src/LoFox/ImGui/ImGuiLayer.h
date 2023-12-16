#pragma once

#include "LoFox/Core/Layer.h"

#include "LoFox/Events/ApplicationEvent.h"
#include "LoFox/Events/KeyEvent.h"
#include "LoFox/Events/MouseEvent.h"

namespace LoFox {

	class ImGuiLayer : public Layer {

	public:
		ImGuiLayer() {}
		~ImGuiLayer() = default;

		virtual void Begin() = 0;
		virtual void End() = 0;

		void BlockEvents(bool block) { m_BlockEvents = block; }

		void SetStyle();

		static Ref<ImGuiLayer> Create();
	protected:
		void SetupViewport();
	protected:
		bool m_BlockEvents = true;
	};
}
