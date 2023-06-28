#include "lfpch.h"
#include "LoFox/Core/Application.h"

#include "LoFox/Core/Log.h"

#include <vulkan/vulkan.h>

#include "LoFox/Utils/VulkanUtils.h"

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

	// Should only be used when LF_USE_VULKAN_VALIDATION_LAYERS is defined
	const std::vector<const char*> Application::s_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	Application::Application(const ApplicationSpec& spec)
		: m_Spec(spec) {

		LF_OVERSPECIFY("Creating application named \"{0}\":\n", m_Spec.Name);

		m_Window = Window::Create({ m_Spec.Name, 1720, 960 });
		InitVulkan();

		LF_OVERSPECIFY("Creation of application \"{0}\" complete.\n", m_Spec.Name);
	}

	Application::~Application() {

		LF_OVERSPECIFY("Destroying application named \"{0}\"", m_Spec.Name);

		LF_OVERSPECIFY("Destroying Vulkan instance");

		#ifdef LF_USE_VULKAN_VALIDATION_LAYERS
		DestroyVulkanDebugUtilsMessengerEXT(m_VulkanInstance, m_VulkanDebugMessenger, nullptr);
		#endif
		vkDestroyInstance(m_VulkanInstance, nullptr);

		LF_OVERSPECIFY("Finished destruction of application named \"{0}\"", m_Spec.Name);
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

	void Application::InitVulkan() {

		LF_OVERSPECIFY("Creating Vulkan instance");
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
		Utils::PopulateDebugMessageCreateInfo(debugMessengerCreateInfo, VulkanMessageCallback);
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

		// Setting up m_VulkanDebugMessenger (for entire runtime of the application)
		#ifdef LF_USE_VULKAN_VALIDATION_LAYERS
		VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo;
		Utils::PopulateDebugMessageCreateInfo(messengerCreateInfo, VulkanMessageCallback);
		LF_CORE_ASSERT(CreateVulkanDebugMessengerEXT(m_VulkanInstance, &messengerCreateInfo, nullptr, &m_VulkanDebugMessenger) == VK_SUCCESS, "Failed to set up debug messenger!");
		#endif

		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanPhysicalDevices(m_VulkanInstance);
		#endif

		m_VulkanPhysicalDevice = Utils::PickVulkanPhysicalDevice(m_VulkanInstance);
	}
}