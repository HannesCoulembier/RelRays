#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

#include "LoFox/Events/Event.h"

struct GLFWwindow; // Forward declaration

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
		void Shutdown();

		void CreateVulkanSurface(VkInstance instance, VkSurfaceKHR* surface);

		void OnUpdate();
		void SetEventCallback(const std::function<void(Event&)>& callback) { m_WindowData.WindowEventCallback = callback; }

		bool IsMinimized() { return m_WindowData.Width == 0 || m_WindowData.Height == 0; }
		void SetTitle(const std::string& title);

		void GetFramebufferSize(int* width, int* height) const;
		WindowSpec GetSpec() { return m_Spec; }

		inline void* GetWindowHandle() { return m_WindowHandle; }

		static Ref<Window> Create(const WindowSpec& spec);
	private:
		void Init();
	private:
		GLFWwindow* m_WindowHandle = nullptr;
		WindowSpec m_Spec;

		struct WindowData {

			uint32_t Width, Height;

			std::function<void(Event&)> WindowEventCallback;
		};

		WindowData m_WindowData;
	};
}