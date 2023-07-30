#include <LoFox.h>
#include <LoFox/Core/EntryPoint.h>

#include "RaytraceExampleLayer.h"
#include "InstancingExampleLayer.h"

namespace LoFox {

	class SandboxApp : public Application {

	public:
		SandboxApp(const ApplicationSpec& spec)
			: Application(spec) {
			
			// Examples: Only use 1 at a time!
			PushLayer(CreateRef<RaytraceExampleLayer>());
			// PushLayer(CreateRef<InstancingExampleLayer>());
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
