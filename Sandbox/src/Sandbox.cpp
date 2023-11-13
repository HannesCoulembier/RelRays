#include <LoFox.h>
#include <LoFox/Core/EntryPoint.h>

#include <RelRays.h>

#include "RaytraceExampleLayer.h"
// #include "InstancingExampleLayer.h"
#include "OpenGLDevelopment.h"

namespace LoFox {

	class SandboxApp : public Application {

	public:
		SandboxApp(const ApplicationSpec& spec)
			: Application(spec) {
			
			RelRays::RelRaysLayer test = RelRays::RelRaysLayer();

			// Examples: Only use 1 at a time!
			PushLayer(CreateRef<RaytraceExampleLayer>());
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
