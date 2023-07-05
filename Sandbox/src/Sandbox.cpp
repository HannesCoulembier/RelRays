#include <LoFox.h>
#include <LoFox/Core/EntryPoint.h>

namespace LoFox {

	class ExampleLayer : public Layer {

	public:
		ExampleLayer() {}
		~ExampleLayer() {}

		void OnAttach() {}
		void OnDetach() {}

		void OnUpdate(float ts) {

			static float avgFPS = 0;
			static uint32_t frames = 0;
			frames++;
			float FPS = 1.0f / ts;
			avgFPS += (FPS - avgFPS) / (float)frames;
			static float maxFPS = 0;
			maxFPS = std::max(maxFPS, FPS);
			Application::GetInstance().GetActiveWindow()->SetTitle("Sandbox Application: " + std::to_string(FPS) + " FPS (avg: " + std::to_string(avgFPS) + ", max: " + std::to_string(maxFPS) + ")");

			if (!Application::GetInstance().GetActiveWindow()->IsMinimized()) {

				Renderer::StartFrame();

				Renderer::SubmitFrame();
			}

			// Logger test
			/*
			LF_CORE_TRACE("TEST");
			LF_CORE_INFO("TEST");
			LF_CORE_WARN("TEST");
			LF_CORE_ERROR("TEST");
			LF_CORE_CRITICAL("TEST");

			LF_TRACE("TEST");
			LF_INFO("TEST");
			LF_WARN("TEST");
			LF_ERROR("TEST");
			LF_CRITICAL("TEST");
			*/

			// Input test
			/*
			LF_INFO("{0}", LoFox::Input::IsKeyPressed(LoFox::KeyCode::A));
			LF_INFO("{0}", LoFox::Input::GetMousePosition());
			LF_INFO("{0}", LoFox::Input::IsMouseButtonPressed(LoFox::MouseCode::Button0));
			*/
		}

		void OnEvent(LoFox::Event& event) {

			LoFox::EventDispatcher dispatcher(event);
			
			// Event test
			/*
			if (event.GetEventType() == LoFox::EventType::KeyPressed)
				LF_INFO("Pressed key {0}", static_cast<LoFox::KeyPressedEvent&>(event).GetKeyCode());
			*/
		}
	};

	class SandboxApp : public Application {

	public:
		SandboxApp(const ApplicationSpec& spec)
			: Application(spec) {
			
			PushLayer(CreateRef<ExampleLayer>());
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
