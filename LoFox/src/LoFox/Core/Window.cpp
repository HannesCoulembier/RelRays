#include "lfpch.h"
#include "LoFox/Core/Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace LoFox {

	namespace Utils {

		std::vector<const char*> GetRequiredGLFWExtensions() {

			uint32_t glfwExtensionCount;
			const char** glfwExtensions;
			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
			return requiredExtensions;
		}
	}

	static uint32_t windowCount = 0;

	Scope<Window> Window::Create(const WindowSpec& spec) {

		return CreateScope<Window>(spec);
	}

	Window::Window(const WindowSpec& spec) {
		Init(spec);
	}

	Window::~Window() {
		Shutdown();
	}

	void Window::Init(const WindowSpec& spec) {

		if (windowCount == 0)
			glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_WindowHandle = glfwCreateWindow(spec.Width, spec.Height, spec.Title.c_str(), nullptr, nullptr);
		windowCount++;
	}

	void Window::Shutdown() {

		glfwDestroyWindow(m_WindowHandle);
		windowCount--;
		if (windowCount == 0)
			glfwTerminate();
	}
}