#include "lfpch.h"
#include "LoFox/Core/Window.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "LoFox/Events/ApplicationEvent.h"
#include "LoFox/Events/RenderEvent.h"

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

	Ref<Window> Window::Create(const WindowSpec& spec) {

		return CreateRef<Window>(spec);
	}

	Window::Window(const WindowSpec& spec) 
		: m_Spec(spec) {

		m_WindowData.Width = m_Spec.Width;
		m_WindowData.Height = m_Spec.Height;

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

		m_WindowHandle = glfwCreateWindow(m_Spec.Width, m_Spec.Height, m_Spec.Title.c_str(), nullptr, nullptr);
		windowCount++;

		glfwSetWindowUserPointer(m_WindowHandle, &m_WindowData);

		// Set glfw callbacks
		glfwSetWindowSizeCallback(m_WindowHandle, [](GLFWwindow* window, int width, int height) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.WindowEventCallback(event);
		});

		glfwSetFramebufferSizeCallback(m_WindowHandle, [](GLFWwindow* window, int width, int height) {
			
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			FramebufferResizeEvent event(width, height);
			data.RenderEventCallback(event);
		});

		glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* window) {

			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.WindowEventCallback(event);
		});
	}

	void Window::Shutdown() {

		glfwDestroyWindow(m_WindowHandle);
		windowCount--;
		if (windowCount == 0)
			glfwTerminate();
	}

	void Window::OnUpdate() {

		glfwPollEvents();
	}

	void Window::CreateVulkanSurface(VkInstance instance, VkSurfaceKHR* surface) {

		LF_CORE_ASSERT(glfwCreateWindowSurface(instance, m_WindowHandle, nullptr, surface) == VK_SUCCESS, "Failed to create window surface!");
	}

	void Window::GetFramebufferSize(int* width, int* height) const {
		return glfwGetFramebufferSize(m_WindowHandle, width, height);
	}
}