#include <LoFox.h>
#include <LoFox/Core/EntryPoint.h>

#include "RelRaysDevLayer.h"

namespace LoFox {

	class SandboxApp : public Application {

	public:
		SandboxApp(const ApplicationSpec& spec)
			: Application(spec) {

			// Examples: Only use 1 at a time!
			PushLayer(CreateRef<RelRaysDevLayer>());
		}

		~SandboxApp() {

		}
	};

	Application* CreateApplication() {

		ApplicationSpec spec;
		spec.Name = "Sandbox Application";
		return new SandboxApp(spec);
	}
}
