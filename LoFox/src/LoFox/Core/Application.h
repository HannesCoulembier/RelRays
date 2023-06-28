#pragma once

#include "LoFox/Core/Core.h"

#include "vulkan/vulkan.h"

#include "LoFox/Core/Window.h"

int main(int argc, char** argv);

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
		void InitVulkan();
	private:
		Scope<Window> m_Window;
		VkInstance m_VulkanInstance = nullptr;
		VkPhysicalDevice m_VulkanPhysicalDevice = nullptr;
		VkDebugUtilsMessengerEXT m_VulkanDebugMessenger = nullptr;
		ApplicationSpec m_Spec;

		static const std::vector<const char*> s_ValidationLayers;

		static Application* s_Instance;
		friend int ::main(int argc, char** argv);
	};

	// To be defined in client
	Application* CreateApplication();
}