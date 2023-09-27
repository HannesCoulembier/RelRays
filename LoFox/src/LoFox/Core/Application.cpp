#include "lfpch.h"
#include "LoFox/Core/Application.h"

#include "LoFox/Core/Input.h"

#include "LoFox/Renderer/Renderer.h"

namespace LoFox {

	Application* Application::s_Instance = nullptr;

	Application::Application(const ApplicationSpec& spec)
		: m_Spec(spec) {

		LF_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

		LF_OVERSPECIFY("Creating application named \"{0}\":\n", m_Spec.Name);

		Input::SetKeyboard(Keyboard::BelgianPeriod);

		// Creating window
		m_Window = Window::Create({ m_Spec.Name, 1720, 960 }); // TODO: Move screen size settings to ApplicationSpec.
		m_Window->SetEventCallback(LF_BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init(m_Window);

		LF_OVERSPECIFY("Creation of application \"{0}\" complete.\n", m_Spec.Name);
	}

	void Application::Run() {

		m_IsRunning = true;
		while (m_IsRunning) {

			m_Window->OnUpdate(); // Processes events from the window (minimizing, resizing, closing, ...).

			if (!m_Window->IsMinimized()) // Trying to render something when the window is minimized causes a crash.
				OnUpdate(); // Keeps track of the timestep and updates all the layers.
		}

		// TODO: re-enable renderer
		// Renderer::WaitIdle(); // TODO: Move to destructor.
	}

	void Application::OnUpdate() {

		float time = Time::GetTime();
		float timestep = time - m_LastFrameTime;
		m_LastFrameTime = time;

		for (auto& layer : m_LayerStack)
			layer->OnUpdate(timestep);
	}

	Application::~Application() {

		LF_OVERSPECIFY("Destroying application named \"{0}\"", m_Spec.Name);

		for (auto& layer : m_LayerStack)
			layer->OnDetach();

		Renderer::Shutdown();

		m_Window->Shutdown();

		LF_OVERSPECIFY("Finished destruction of application named \"{0}\"", m_Spec.Name);
	}

	void Application::OnEvent(Event& event) {

		EventDispatcher dispatcher(event); // Dispatches events to functions based on their type (WindowCloseEvent, WindowResizeEvent, ...).
		dispatcher.Dispatch<WindowCloseEvent>(LF_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(LF_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto layer : m_LayerStack) { // Presents event to the layers, until a layer marks it handled. (Clicking on a button -> all layers behind the button don't need to recieve the event).

			if (event.Handled)
				break;
			layer->OnEvent(event);
		}
	}

	bool Application::OnWindowResize(WindowResizeEvent& event) {

		Renderer::OnResize(event.GetWidth(), event.GetHeight());
		return true;
	}

	bool Application::OnWindowClose(WindowCloseEvent& event) {

		LF_OVERSPECIFY("Closing application named \"{0}\"", m_Spec.Name);
		m_IsRunning = false;
		return true;
	}
}