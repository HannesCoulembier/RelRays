#pragma once

#include "LoFox/Core/Core.h"

#include "LoFox/Core/Window.h"
#include "LoFox/Core/Layer.h"
#include "LoFox/Renderer/RenderContext.h"

#include "LoFox/Events/Event.h"
#include "LoFox/Events/ApplicationEvent.h"

int main(int argc, char** argv);

namespace LoFox {

	struct ApplicationSpec {

		std::string Name = "LoFox application";
		std::string WorkingDirectory;
	};

	class Application {
		
	public:
		Application(const ApplicationSpec& spec);
		~Application();

		void OnEvent(Event& event);

		void PushLayer(Ref<Layer> layer) { m_LayerStack.emplace_back(layer); layer->OnAttach(this); }

		void Run();

		inline Ref<Window> GetActiveWindow() { return m_Window; }
	private:
		bool OnWindowResize(WindowResizeEvent& event);
		bool OnWindowClose(WindowCloseEvent& event);
	private:
		ApplicationSpec m_Spec;
		Ref<Window> m_Window;
		Ref<RenderContext> m_RenderContext;
		std::vector<Ref<Layer>> m_LayerStack;

		float m_LastFrameTime = 0.0f;
		bool m_IsRunning = false;

		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in client
	Application* CreateApplication();
}