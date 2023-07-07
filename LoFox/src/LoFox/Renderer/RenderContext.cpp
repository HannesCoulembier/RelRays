#include "lfpch.h"
#include "LoFox/Renderer/RenderContext.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // glm is made for OpenGL that has depth range -1 to 1, we need 0 to 1
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "LoFox/Renderer/Shader.h"
#include "LoFox/Renderer/Buffer.h"
#include "LoFox/Renderer/SwapChain.h"
#include "LoFox/Renderer/Image.h"
#include "LoFox/Renderer/Pipeline.h"

#include "LoFox/Renderer/DebugMessenger.h"

#include "LoFox/Core/Settings.h"
#include "LoFox/Utils/VulkanUtils.h"
#include "LoFox/Utils/Utils.h"

#include "LoFox/Events/RenderEvent.h"

#include "LoFox/Renderer/Renderer.h"

#include "LoFox/Core/Application.h"

namespace LoFox {

	VkInstance RenderContext::Instance;
	VkPhysicalDevice RenderContext::PhysicalDevice;
	VkDevice RenderContext::LogicalDevice;
	VkSurfaceKHR RenderContext::Surface;

	VkQueue RenderContext::GraphicsQueueHandle;
	VkQueue RenderContext::PresentQueueHandle;

	Ref<DebugMessenger> RenderContext::m_DebugMessenger;
	Ref<Window> RenderContext::m_Window;

	VkCommandPool RenderContext::CommandPool;
	VkCommandBuffer RenderContext::MainCommandBuffer;

	void RenderContext::Init(Ref<Window> window) {

		m_Window = window;

		LF_OVERSPECIFY("Creating Vulkan instance");
		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanExtensions();
		Utils::ListAvailableVulkanLayers();
		#endif

		InitInstance();
		m_DebugMessenger = DebugMessenger::Create();

		// Setting up Surface
		m_Window->CreateVulkanSurface(Instance, &Surface);

		InitDevices();

	}
	
	void RenderContext::Shutdown() {

		vkDestroyCommandPool(LogicalDevice, CommandPool, nullptr);

		m_DebugMessenger->Shutdown();

		vkDestroyDevice(LogicalDevice, nullptr);

		vkDestroySurfaceKHR(Instance, Surface, nullptr);

		vkDestroyInstance(Instance, nullptr);
	}

	void RenderContext::InitInstance() {

		VkApplicationInfo appInfo;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = NULL;
		appInfo.pApplicationName = m_Window->GetSpec().Title.c_str();
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
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
		Utils::PopulateDebugMessageCreateInfo(debugMessengerCreateInfo, m_DebugMessenger->GetMessageCallback());
		instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerCreateInfo;
		#endif

		LF_CORE_ASSERT(vkCreateInstance(&instanceCreateInfo, nullptr, &Instance) == VK_SUCCESS, "Failed to create instance!");
	}

	void RenderContext::InitDevices() {

		// Pick physical device (= GPU to use)
		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanPhysicalDevices(Instance);
		#endif

		PhysicalDevice = Utils::PickVulkanPhysicalDevice(Instance, Surface);

		// Create logical device
		// Figuring out what queues we want
		Utils::QueueFamilyIndices indices = Utils::IdentifyVulkanQueueFamilies(PhysicalDevice, Surface);

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

	VkCommandBuffer RenderContext::BeginSingleTimeCommandBuffer() {

		VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.commandPool = CommandPool;
		commandBufferAllocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(LogicalDevice, &commandBufferAllocInfo, &commandBuffer);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		return commandBuffer;
	}

	void RenderContext::EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer) {

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(GraphicsQueueHandle, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(GraphicsQueueHandle);

		vkFreeCommandBuffers(LogicalDevice, CommandPool, 1, &commandBuffer);
	}
}