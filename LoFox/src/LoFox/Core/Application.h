#pragma once

#include "LoFox/Core/Core.h"

#include "vulkan/vulkan.h"

#include "LoFox/Core/Window.h"
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

		void Run();

		inline Window& GetWindow() { return *m_Window; }
		
	private:
		ApplicationSpec m_Spec;
		Ref<Window> m_Window;
		Ref<RenderContext> m_RenderContext;

		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in client
	Application* CreateApplication();
}