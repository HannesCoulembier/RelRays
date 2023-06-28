#include "lfpch.h"
#include "LoFox/Core/Application.h"

#include "LoFox/Core/Log.h"

#include <vulkan/vulkan.h>

#ifdef LF_DEBUG
	#define LF_USE_VULKAN_VALIDATION_LAYERS
#endif

namespace LoFox {

	// proxy functions: -----------
	// vkCreateDebugUtilsMessengerEXT is an extension that needs to be loaded. This proxy function loads and then runs it as if it was there all along.
	VkResult CreateVulkanDebugMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func == nullptr)
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}

	// vkDestroyDebugUtilsMessengerEXT is an extension that needs to be loaded. This proxy function loads and then runs it as if it was there all along.
	void DestroyVulkanDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {

		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(instance, debugMessenger, pAllocator);
	}
	// proxy functions ------------

	static VKAPI_ATTR VkBool32 VKAPI_CALL VulkanMessageCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		#ifndef LF_BE_OVERLYSPECIFIC
		if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			return VK_FALSE;
		#endif

		switch (messageSeverity) {

			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:		{ LF_CORE_INFO("Vulkan [INFO]: " + (std::string)pCallbackData->pMessage); break; }
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:	{ LF_CORE_INFO("Vulkan [VERBOSE]: " + (std::string)pCallbackData->pMessage); break; }
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:	{ LF_CORE_WARN("Vulkan [WARNING]: " + (std::string)pCallbackData->pMessage); break; }
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:		{ LF_CORE_ERROR("Vulkan [ERROR]: " + (std::string)pCallbackData->pMessage); break; }
		}
		
		return VK_FALSE; // When VK_TRUE is returned, Vulkan will abort the call that made this callback
	}

	namespace Utils {

		std::vector<const char*> GetRequiredVulkanExtensions() {

			std::vector<const char*> extensions = GetRequiredGLFWExtensions();
			#ifdef LF_USE_VULKAN_VALIDATION_LAYERS
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			#endif
			return extensions;
		}

		std::vector<VkExtensionProperties> GetVulkanExtensions() {

			uint32_t extensionCount;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
			return extensions;
		}

		void ListVulkanExtensions() {

			std::vector<VkExtensionProperties> extensions = GetVulkanExtensions();

			std::string message = "All available Vulkan extensions:\n";
			for (const VkExtensionProperties& extension : extensions)
				message += "\t" + (std::string)extension.extensionName + "\n";
			LF_CORE_INFO(message);
		}

		std::vector<VkLayerProperties> GetAvailableVulkanLayers() {

			uint32_t availableLayerCount;
			vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
			std::vector<VkLayerProperties> availableLayers(availableLayerCount);
			vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());
			return availableLayers;
		}

		void ListAvailableVulkanLayers() {

			std::vector<VkLayerProperties> availableLayers = GetAvailableVulkanLayers();

			std::string message = "All available Vulkan layers:\n";
			for (const VkLayerProperties& layer : availableLayers)
				message += "\t" + (std::string)layer.layerName + "\n";
			LF_CORE_INFO(message);
		}

		// Checks if a list of layer names are available
		bool CheckVulkanValidationLayerSupport(const std::vector<const char*>& layers) {

			std::vector<VkLayerProperties> availableLayers = GetAvailableVulkanLayers();

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

		void PopulateDebugMessageCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
			
			VkDebugUtilsMessageSeverityFlagsEXT messageSeverities;
			messageSeverities = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			#ifdef LF_BE_OVERLYSPECIFIC
			messageSeverities |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
			#endif

			createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = messageSeverities;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = VulkanMessageCallback;
			createInfo.pUserData = nullptr;
		}
	}

	// Should only be used when LF_USE_VULKAN_VALIDATION_LAYERS is defined
	const std::vector<const char*> Application::s_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	Application::Application(const ApplicationSpec& spec) {

		LF_OVERSPECIFY("Creating application named \"{0}\":\n", spec.Name);
		InitWindow(spec);
		InitVulkan();
		LF_OVERSPECIFY("Creation of application \"{0}\" complete.\n", spec.Name);
	}

	Application::~Application() {

		#ifdef LF_USE_VULKAN_VALIDATION_LAYERS
		DestroyVulkanDebugUtilsMessengerEXT(m_VulkanInstance, m_DebugMessenger, nullptr);
		#endif
		vkDestroyInstance(m_VulkanInstance, nullptr);
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

		m_Window = Window::Create({ spec.Name, 1720, 960 });
	}

	void Application::InitVulkan() {

		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanExtensions();
		Utils::ListAvailableVulkanLayers();
		#endif

		#ifdef LF_USE_VULKAN_VALIDATION_LAYERS
		LF_CORE_ASSERT(Utils::CheckVulkanValidationLayerSupport(s_ValidationLayers), "Validation layers requested, but not available!");
		#endif

		// Setting up m_VulkanInstance
		VkApplicationInfo appInfo;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = NULL;
		appInfo.pApplicationName = "LoFox Application";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "LoFox";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		auto extensions = Utils::GetRequiredVulkanExtensions();
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
		#ifdef LF_USE_VULKAN_VALIDATION_LAYERS
		instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = s_ValidationLayers.data();

		#ifdef LF_BE_OVERLYSPECIFIC
		// Add debug messenger to instance creation and cleanup
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
		Utils::PopulateDebugMessageCreateInfo(debugMessengerCreateInfo);
		instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugMessengerCreateInfo;
		#endif

		#else
		instanceCreateInfo.enabledLayerCount = 0;
		instanceCreateInfo.pNext = nullptr;
		#endif

		VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &m_VulkanInstance);

		if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}

		// Setting up m_DebugMessenger (for entire runtime of the application)
		#ifdef LF_USE_VULKAN_VALIDATION_LAYERS
		VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo;
		Utils::PopulateDebugMessageCreateInfo(messengerCreateInfo);
		LF_CORE_ASSERT(CreateVulkanDebugMessengerEXT(m_VulkanInstance, &messengerCreateInfo, nullptr, &m_DebugMessenger) == VK_SUCCESS, "Failed to set up debug messenger!");
		#endif
	}
}