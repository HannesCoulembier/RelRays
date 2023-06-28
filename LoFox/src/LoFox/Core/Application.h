#pragma once

#include "LoFox/Core/Core.h"

#include "vulkan/vulkan.h"

int main(int argc, char** argv);

struct GLFWwindow;

namespace LoFox {

	struct ApplicationSpec {

		std::string Name = "LoFox application";
		std::string WorkingDirectory;
	};

	class Application {
		
	public:
		Application(const ApplicationSpec& spec);
		~Application();

		void Run();
	private:
		void InitWindow(const ApplicationSpec& spec);
		void InitVulkan();
	private:
		GLFWwindow* m_WindowHandle = nullptr;
		VkInstance m_VulkanInstance = nullptr;
		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;

		static const std::vector<const char*> s_ValidationLayers;

		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in client
	Application* CreateApplication();
}