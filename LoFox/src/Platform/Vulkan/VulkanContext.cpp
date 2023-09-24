#include "lfpch.h"
#include "Platform/Vulkan/VulkanContext.h"

#include <GLFW/glfw3.h>

#include "LoFox/Core/Application.h"
#include "Platform/Vulkan/Utils.h"

namespace LoFox {

	void VulkanContext::Init(GLFWwindow* windowHandle) {

		LF_CORE_ASSERT(windowHandle, "Window handle is null!");
		m_WindowHandle = windowHandle;

		LF_OVERSPECIFY("Creating Vulkan instance");
		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanExtensions();
		Utils::ListAvailableVulkanLayers();
		#endif

		InitInstance();
		m_DebugMessenger = DebugMessenger::Create();
		InitSurface();
		InitDevices();
	}

	void VulkanContext::Shutdown() {

		vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);
		m_DebugMessenger->Shutdown();
		vkDestroyDevice(LogicalDevice, nullptr);
		vkDestroySurfaceKHR(Instance, Surface, nullptr);
		vkDestroyInstance(Instance, nullptr);
	}

	void VulkanContext::PresentFrame() {

	}

	void VulkanContext::InitInstance() {

		VkApplicationInfo appInfo;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = NULL;
		appInfo.pApplicationName = Application::GetInstance().GetSpec().Name.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(LF_VERSION_MAJOR, LF_VERSION_MINOR, LF_VERSION_PATCH);
		appInfo.pEngineName = LF_ENGINE_NAME;
		appInfo.engineVersion = VK_MAKE_VERSION(LF_VERSION_MAJOR, LF_VERSION_MINOR, LF_VERSION_PATCH);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		auto extensions = Utils::GetRequiredVulkanExtensions();
		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
		instanceCreateInfo.enabledLayerCount = (uint32_t)(DebugMessenger::ValidationLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = DebugMessenger::ValidationLayers.data();
		instanceCreateInfo.pNext = nullptr;

		#ifdef LF_BE_OVERLYSPECIFIC
		// Add debug messenger to instance creation and cleanup
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = Utils::MakeDebugMessageCreateInfo(DebugMessenger::GetMessageCallback());
		instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerCreateInfo;
		#endif

		LF_CORE_ASSERT(vkCreateInstance(&instanceCreateInfo, nullptr, &Instance) == VK_SUCCESS, "Failed to create instance!");
	}

	void VulkanContext::InitSurface() {

		LF_CORE_ASSERT(glfwCreateWindowSurface(Instance, m_WindowHandle, nullptr, &Surface) == VK_SUCCESS, "Failed to create window surface!");
	}

	void VulkanContext::InitDevices() {

		// Pick physical device (= GPU to use)
		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanPhysicalDevices(VulkanContext::Instance);
		#endif

		PhysicalDevice = Utils::PickVulkanPhysicalDevice(VulkanContext::Instance, VulkanContext::Surface);

		// Create logical device
		// Figuring out what queues we want
		Utils::QueueFamilyIndices indices = Utils::IdentifyVulkanQueueFamilies(PhysicalDevice, VulkanContext::Surface);

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
		logicalDeviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo logicalDeviceCreateInfo{};
		logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		logicalDeviceCreateInfo.queueCreateInfoCount = (uint32_t)(queueCreateInfos.size());
		logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		logicalDeviceCreateInfo.pEnabledFeatures = &logicalDeviceFeatures;
		logicalDeviceCreateInfo.enabledExtensionCount = (uint32_t)(Utils::requiredVulkanDeviceExtensions.size());
		logicalDeviceCreateInfo.ppEnabledExtensionNames = Utils::requiredVulkanDeviceExtensions.data();
		logicalDeviceCreateInfo.enabledLayerCount = (uint32_t)(DebugMessenger::ValidationLayers.size());
		logicalDeviceCreateInfo.ppEnabledLayerNames = DebugMessenger::ValidationLayers.data();
		logicalDeviceCreateInfo.pNext = nullptr;

		LF_CORE_ASSERT(vkCreateDevice(PhysicalDevice, &logicalDeviceCreateInfo, nullptr, &LogicalDevice) == VK_SUCCESS, "Failed to create logical device!");

		// Retrieve queue handles
		vkGetDeviceQueue(LogicalDevice, indices.GraphicsFamilyIndex, 0, &GraphicsQueueHandle);
		vkGetDeviceQueue(LogicalDevice, indices.PresentFamilyIndex, 0, &PresentQueueHandle);

		// Create Commandpool
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = indices.GraphicsFamilyIndex;

		LF_CORE_ASSERT(vkCreateCommandPool(LogicalDevice, &commandPoolCreateInfo, nullptr, &CommandPool) == VK_SUCCESS, "Failed to create command pool!");

		// Create MainCommandBuffer
		VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.commandBufferCount = 1;
		commandBufferAllocInfo.commandPool = CommandPool;
		// commandBufferAllocInfo.level = 
		// commandBufferAllocInfo.pNext = 
		LF_CORE_ASSERT(vkAllocateCommandBuffers(LogicalDevice, &commandBufferAllocInfo, &MainCommandBuffer) == VK_SUCCESS, "Failed to allocate main command buffer!");
		

	}
}
