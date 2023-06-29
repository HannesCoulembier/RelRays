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

	Window::Window(const WindowSpec& spec) 
		: m_Spec(spec) {

		LF_OVERSPECIFY("Creating {0}x{1} window named \"{2}\"", m_Spec.Width, m_Spec.Height, m_Spec.Title);
		Init();
	}

	Window::~Window() {

		LF_OVERSPECIFY("Destroying window named \"{0}\"", m_Spec.Title);
		
		Shutdown();
	}

	void Window::Init() {

		if (windowCount == 0)
			glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_WindowHandle = glfwCreateWindow(m_Spec.Width, m_Spec.Height, m_Spec.Title.c_str(), nullptr, nullptr);
		windowCount++;
	}

	void Window::Shutdown() {

		glfwDestroyWindow(m_WindowHandle);
		windowCount--;
		if (windowCount == 0)
			glfwTerminate();
	}

	void Window::CreateVulkanSurface(VkInstance instance, VkSurfaceKHR* surface) {

		LF_CORE_ASSERT(glfwCreateWindowSurface(instance, m_WindowHandle, nullptr, surface) == VK_SUCCESS, "Failed to create window surface!");
	}

	void Window::GetFramebufferSize(int* width, int* height) const {
		return glfwGetFramebufferSize(m_WindowHandle, width, height);
	}
}