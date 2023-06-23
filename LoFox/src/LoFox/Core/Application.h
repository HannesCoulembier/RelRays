#pragma once

#include "Core.h"

namespace LoFox {

	struct ApplicationSpec {

		std::string Name = "LoFox application";
		std::string WorkingDirectory;
	};

	class Application {
		
	public:
		Application(const ApplicationSpec& spec);
		~Application() = default;

		void Run();
	private:

	};

	// To be defined in client
	Application* CreateApplication();
}