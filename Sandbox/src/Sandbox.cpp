#include <LoFox.h>
#include <LoFox/Core/EntryPoint.h>

#include "RaytraceExampleLayer.h"
// #include "InstancingExampleLayer.h"
#include "OpenGLDevelopment.h"
#include "RelRaysDevLayer.h"

namespace LoFox {

	class SandboxApp : public Application {

	public:
		SandboxApp(const ApplicationSpec& spec)
			: Application(spec) {

			// Examples: Only use 1 at a time!
			// PushLayer(CreateRef<RaytraceExampleLayer>());
			PushLayer(CreateRef<RelRaysDevLayer>());
			// PushLayer(CreateRef<InstancingExampleLayer>());
			// PushLayer(CreateRef<OpenGLDevLayer>());
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
