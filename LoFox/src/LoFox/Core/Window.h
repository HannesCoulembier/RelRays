#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace LoFox {

	namespace Utils {

		std::vector<const char*> GetRequiredGLFWExtensions();
	}

	struct WindowSpec {

		std::string Title;
		uint32_t Width, Height;
	};

	class Window {

	public:
		Window(const WindowSpec& spec);
		~Window();

		void CreateVulkanSurface(VkInstance instance, VkSurfaceKHR* surface);

		void GetFramebufferSize(int* width, int* height) const;

		static Scope<Window> Create(const WindowSpec& spec);
	private:
		void Init();
		void Shutdown();
	private:
		GLFWwindow* m_WindowHandle = nullptr;
		WindowSpec m_Spec;
	};
}