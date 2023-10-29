#pragma once

#include "LoFox/Core/Core.h"

#include "LoFox/Events/Event.h"

struct GLFWwindow; // Forward declaration

namespace LoFox {

	struct WindowSpec {

		std::string Title;
		uint32_t Width, Height;
	};

	struct WindowData {

		uint32_t Width, Height;

		std::function<void(Event&)> WindowEventCallback;
	};

	class Window {

	public:
		Window(const WindowSpec& spec);
		void Shutdown();

		void OnUpdate();
		void SetEventCallback(const std::function<void(Event&)>& callback) { m_WindowData.WindowEventCallback = callback; }

		bool IsMinimized() { return m_WindowData.Width == 0 || m_WindowData.Height == 0; }
		void SetTitle(const std::string& title);

		void GetFramebufferSize(int* width, int* height) const;
		WindowSpec GetSpec() { return m_Spec; }

		inline void* GetWindowHandle() { return m_WindowHandle; }
		inline WindowData GetWindowData() { return m_WindowData; }

		static Ref<Window> Create(const WindowSpec& spec);
	private:
		void Init();
	private:
		GLFWwindow* m_WindowHandle = nullptr;
		WindowSpec m_Spec;

		WindowData m_WindowData;
	};
}