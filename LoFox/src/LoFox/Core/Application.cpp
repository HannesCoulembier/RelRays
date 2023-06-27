#include "lfpch.h"
#include "LoFox/Core/Application.h"

#include "LoFox/Core/Log.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

// Will give A LOT of debug info (that you propably don't need)
// #define LF_BE_OVERLYSPECIFIC

#ifdef LF_DEBUG
	#define LF_USE_VULKAN_VALIDATION_LAYERS
#endif

namespace LoFox {

	namespace Utils {
		
		void ListVulkanExtensions() {

			// Gets all available extensions
			uint32_t extensionCount;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

			// Lists them
			std::string message = "All available Vulkan extensions:\n";
			for (const VkExtensionProperties& extension : extensions)
				message += "\t" + (std::string)extension.extensionName + "\n";
			LF_CORE_INFO(message);
		}

		void ListAvailableVulkanLayers() {

			// Gets all available layers
			uint32_t availableLayerCount;
			vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
			std::vector<VkLayerProperties> availableLayers(availableLayerCount);
			vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

			// Lists them
			std::string message = "All available Vulkan layers:\n";
			for (const VkLayerProperties& layer : availableLayers)
				message += "\t" + (std::string)layer.layerName + "\n";
			LF_CORE_INFO(message);
		}

		// Checks if a list of layer names are available
		bool CheckVulkanValidationLayerSupport(const std::vector<const char*>& layers) {

			// Gets all available layers
			uint32_t availableLayerCount;
			vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
			std::vector<VkLayerProperties> availableLayers(availableLayerCount);
			vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

			// Checks every layername against all available layers names
			for (const char* layerName : layers) {
				bool layerFound = false;

				for (const const VkLayerProperties& availableLayerProperties : availableLayers) {

					if (strcmp(layerName, availableLayerProperties.layerName) == 0) {
						layerFound = true;
						break;
					}
				}

				if (!layerFound)
					return false;
			}
			return true;
		}
	}

	// Should only be used when LF_USE_VULKAN_VALIDATION_LAYERS is defined
	const std::vector<const char*> Application::s_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	Application::Application(const ApplicationSpec& spec) {

		InitWindow(spec);
		InitVulkan();
	}

	Application::~Application() {

		vkDestroyInstance(m_VulkanInstance, nullptr);

		glfwDestroyWindow(m_WindowHandle);
		glfwTerminate();
	}

	void Application::Run() {

		// Logger test
		/*
		LF_CORE_TRACE("TEST");
		LF_CORE_INFO("TEST");
		LF_CORE_WARN("TEST");
		LF_CORE_ERROR("TEST");
		LF_CORE_CRITICAL("TEST");

		LF_TRACE("TEST");
		LF_INFO("TEST");
		LF_WARN("TEST");
		LF_ERROR("TEST");
		LF_CRITICAL("TEST");
		*/
	}

	void Application::InitWindow(const ApplicationSpec& spec) {

		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_WindowHandle = glfwCreateWindow(800, 600, "LoFox Application", nullptr, nullptr);
	}

	void Application::InitVulkan() {

		#ifdef LF_BE_OVERSPECIFIC
		Utils::ListVulkanExtensions();
		Utils::ListAvailableVulkanLayers();
		#endif

		#ifdef LF_USE_VULKAN_VALIDATION_LAYERS
		LF_CORE_ASSERT(Utils::CheckVulkanValidationLayerSupport(s_ValidationLayers), "Validation layers requested, but not available!");
		#endif

		VkApplicationInfo appInfo;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = NULL;
		appInfo.pApplicationName = "LoFox Application";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "LoFox";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		uint32_t glfwExtensionCount;
		const char** glfwExtensions;

		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = glfwExtensionCount - 1;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		#ifdef LF_USE_VULKAN_VALIDATION_LAYERS
		createInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
		createInfo.ppEnabledLayerNames = s_ValidationLayers.data();
		#else
		createInfo.enabledLayerCount = 0;
		#endif

		VkResult result = vkCreateInstance(&createInfo, nullptr, &m_VulkanInstance);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}
	}
}