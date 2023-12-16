#include "lfpch.h"
#include "LoFox/Core/Window.h"

#include <GLFW/glfw3.h>

#include "LoFox/Renderer/Renderer.h"
#include "LoFox/Renderer/GraphicsContext.h"

#include "LoFox/Core/Input.h"

#include "LoFox/Events/KeyEvent.h"
#include "LoFox/Events/ApplicationEvent.h"
#include "LoFox/Events/MouseEvent.h"
#include "LoFox/Events/RenderEvent.h"

namespace LoFox {

	static uint32_t windowCount = 0;

	Ref<Window> Window::Create(const WindowSpec& spec) {

		Ref<Window> window = CreateRef<Window>(spec);
		GraphicsContext::Init(window);
		return window;
	}

	Window::Window(const WindowSpec& spec) 
		: m_Spec(spec) {

		m_WindowData.Width = m_Spec.Width;
		m_WindowData.Height = m_Spec.Height;

		LF_OVERSPECIFY("Creating {0}x{1} window named \"{2}\"", m_Spec.Width, m_Spec.Height, m_Spec.Title);

		Init();
	}

	void Window::Init() {

		if (windowCount == 0)
			glfwInit();

		#if defined(LF_DEBUG)
			if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
				glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
		#endif
		if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
			glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
		else
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_WindowHandle = glfwCreateWindow(m_Spec.Width, m_Spec.Height, m_Spec.Title.c_str(), nullptr, nullptr);
		windowCount++;

		// Provides glfw callbacks with m_WindowData
		glfwSetWindowUserPointer(m_WindowHandle, &m_WindowData);

		// Set glfw callbacks
		glfwSetWindowSizeCallback(m_WindowHandle, [](GLFWwindow* window, int width, int height) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.WindowEventCallback(event);
		});

		glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* window) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.WindowEventCallback(event);
		});

		glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow* window, int key, int scancode, int action, int mods) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {

				case GLFW_PRESS: {

					KeyPressedEvent event(Input::glfwToKeyCode(key), false);
					data.WindowEventCallback(event);
					break;
				}
				case GLFW_RELEASE: {

					KeyReleasedEvent event(Input::glfwToKeyCode(key));
					data.WindowEventCallback(event);
					break;
				}
				case GLFW_REPEAT: {

					KeyPressedEvent event(Input::glfwToKeyCode(key), true);
					data.WindowEventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(m_WindowHandle, [](GLFWwindow* window, unsigned int keycode) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(Input::glfwToKeyCode(keycode));
			data.WindowEventCallback(event);
		});

		glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow* window, int button, int action, int mods) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action) {

				case GLFW_PRESS: {

					MouseButtonPressedEvent event(static_cast<MouseCode>(button));
					data.WindowEventCallback(event);
					break;
				}
				case GLFW_RELEASE: {

					MouseButtonReleasedEvent event(static_cast<MouseCode>(button));
					data.WindowEventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_WindowHandle, [](GLFWwindow* window, double xOffset, double yOffset) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.WindowEventCallback(event);
		});

		glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow* window, double xPos, double yPos) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.WindowEventCallback(event);
		});
	}

	void Window::Shutdown() {

		LF_OVERSPECIFY("Destroying window named \"{0}\"", m_Spec.Title);

		GraphicsContext::Shutdown();

		glfwDestroyWindow(m_WindowHandle);
		windowCount--;
		if (windowCount == 0)
			glfwTerminate();
	}

	void Window::OnUpdate() {

		glfwPollEvents();
		if (!IsMinimized())
			GraphicsContext::PresentFrame();
	}

	void Window::GetFramebufferSize(int* width, int* height) const {
		return glfwGetFramebufferSize(m_WindowHandle, width, height);
	}

	void Window::SetTitle(const std::string& title) {

		glfwSetWindowTitle(m_WindowHandle, title.c_str());
	}
}