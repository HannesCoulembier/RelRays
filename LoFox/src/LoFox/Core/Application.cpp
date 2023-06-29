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

		for (auto imageView : m_VulkanSwapChainImageViews) {
			vkDestroyImageView(m_VulkanLogicalDevice, imageView, nullptr);
		}
		#ifdef LF_USE_VULKAN_VALIDATION_LAYERS
		DestroyVulkanDebugUtilsMessengerEXT(m_VulkanInstance, m_VulkanDebugMessenger, nullptr);
		#endif
		vkDestroySwapchainKHR(m_VulkanLogicalDevice, m_VulkanSwapChain, nullptr);
		vkDestroyDevice(m_VulkanLogicalDevice, nullptr);
		vkDestroySurfaceKHR(m_VulkanInstance, m_VulkanSurface, nullptr);

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

		// Setting up m_VulkanSurface
		m_Window->CreateVulkanSurface(m_VulkanInstance, &m_VulkanSurface);

		// Pick physical device (= GPU to use)
		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanPhysicalDevices(m_VulkanInstance);
		#endif

		m_VulkanPhysicalDevice = Utils::PickVulkanPhysicalDevice(m_VulkanInstance, m_VulkanSurface);

		// Create logical device
		// Figuring out what queues we want
		Utils::QueueFamilyIndices indices = Utils::IdentifyVulkanQueueFamilies(m_VulkanPhysicalDevice, m_VulkanSurface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamilyIndex, indices.PresentFamilyIndex };
		float queuePriority = 1.0f;
		for (auto queueFamily : uniqueQueueFamilies) {

			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Actually setting up the logical device info structs
		VkPhysicalDeviceFeatures logicalDeviceFeatures = {}; // No features needed for now

		VkDeviceCreateInfo logicalDeviceCreateInfo{};
		logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		logicalDeviceCreateInfo.pEnabledFeatures = &logicalDeviceFeatures;
		logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(Utils::requiredVulkanDeviceExtensions.size());
		logicalDeviceCreateInfo.ppEnabledExtensionNames = Utils::requiredVulkanDeviceExtensions.data();

		#ifdef LF_USE_VULKAN_VALIDATION_LAYERS
		logicalDeviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(s_ValidationLayers.size());
		logicalDeviceCreateInfo.ppEnabledLayerNames = s_ValidationLayers.data();
		#else
		logicalDeviceCreateInfo.enabledLayerCount = 0;
		logicalDeviceCreateInfo.pNext = nullptr;
		#endif

		LF_CORE_ASSERT(vkCreateDevice(m_VulkanPhysicalDevice, &logicalDeviceCreateInfo, nullptr, &m_VulkanLogicalDevice) == VK_SUCCESS, "Failed to create logical device!");
		
		// Retrieve queue handles
		vkGetDeviceQueue(m_VulkanLogicalDevice, indices.GraphicsFamilyIndex, 0, &m_GraphicsQueueHandle);
		vkGetDeviceQueue(m_VulkanLogicalDevice, indices.PresentFamilyIndex, 0, &m_PresentQueueHandle);

		// Create SwapChain
		Utils::SwapChainSupportDetails swapChainSupport = Utils::GetSwapChainSupportDetails(m_VulkanPhysicalDevice, m_VulkanSurface);

		VkSurfaceFormatKHR surfaceFormat = Utils::ChooseSwapSurfaceFormat(swapChainSupport.Formats);
		m_SwapChainImageFormat = surfaceFormat.format;
		VkPresentModeKHR presentMode = Utils::ChooseSwapPresentMode(swapChainSupport.PresentModes);
		m_SwapChainExtent = Utils::ChooseSwapExtent(swapChainSupport.Capabilities, GetWindow());

		uint32_t imageCount = swapChainSupport.Capabilities.minImageCount + 1;
		if (swapChainSupport.Capabilities.maxImageCount > 0 && imageCount > swapChainSupport.Capabilities.maxImageCount) // when maxImageCount = 0, there is no limit
			imageCount = swapChainSupport.Capabilities.maxImageCount;

		uint32_t queueFamilyIndices[] = { indices.GraphicsFamilyIndex, indices.PresentFamilyIndex };
		
		VkSwapchainCreateInfoKHR swapChainCreateInfo{};
		swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapChainCreateInfo.surface = m_VulkanSurface;
		swapChainCreateInfo.minImageCount = imageCount;
		swapChainCreateInfo.imageFormat = surfaceFormat.format;
		swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapChainCreateInfo.imageExtent = m_SwapChainExtent;
		swapChainCreateInfo.imageArrayLayers = 1;
		swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (indices.GraphicsFamilyIndex != indices.PresentFamilyIndex) {

			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapChainCreateInfo.queueFamilyIndexCount = 2; // Specifies there are 2 (graphics and present) families that will need to share the swapchain images
			swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		swapChainCreateInfo.preTransform = swapChainSupport.Capabilities.currentTransform;
		swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapChainCreateInfo.presentMode = presentMode;
		swapChainCreateInfo.clipped = VK_TRUE; // Pixels obscured by another window are ignored
		swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		LF_CORE_ASSERT(vkCreateSwapchainKHR(m_VulkanLogicalDevice, &swapChainCreateInfo, nullptr, &m_VulkanSwapChain) == VK_SUCCESS, "Failed to create swap chain!");
		
		// Retrieving the images in the swap chain
		vkGetSwapchainImagesKHR(m_VulkanLogicalDevice, m_VulkanSwapChain, &imageCount, nullptr); // We only specified minImageCount, so swapchain might have created more. We reset imageCount to the actual number of images created.
		m_VulkanSwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_VulkanLogicalDevice, m_VulkanSwapChain, &imageCount, m_VulkanSwapChainImages.data());

		// Create the image views
		m_VulkanSwapChainImageViews.resize(m_VulkanSwapChainImages.size());
		for (uint32_t i = 0; i < m_VulkanSwapChainImages.size(); i++) {

			VkImageViewCreateInfo imageViewCreateInfo{};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = m_VulkanSwapChainImages[i];
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = m_SwapChainImageFormat;
			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			LF_CORE_ASSERT(vkCreateImageView(m_VulkanLogicalDevice, &imageViewCreateInfo, nullptr, &m_VulkanSwapChainImageViews[i]) == VK_SUCCESS, "Failed to create image views!");
		}

	}
}