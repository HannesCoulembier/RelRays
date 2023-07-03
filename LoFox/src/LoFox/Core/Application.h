#pragma once

#include "LoFox/Core/Core.h"

#include "vulkan/vulkan.h"

#include "LoFox/Core/Window.h"
#include "LoFox/Core/Layer.h"
#include "LoFox/Renderer/RenderContext.h"

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

		void PushLayer(Ref<Layer> layer) { m_LayerStack.emplace_back(layer); layer->OnAttach(); }

		void Run();

		inline Window& GetWindow() { return *m_Window; }
		
	private:
		ApplicationSpec m_Spec;
		Ref<Window> m_Window;
		Ref<RenderContext> m_RenderContext;
		std::vector<Ref<Layer>> m_LayerStack;

		float m_LastFrameTime = 0.0f;

		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in client
	Application* CreateApplication();
}