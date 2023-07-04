#include "lfpch.h"
#include "LoFox/Utils/VulkanUtils.h"

namespace LoFox {

	namespace Utils {

		// Populators (populate createInfo structs) -----------------------------------------------------
		void PopulateDebugMessageCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo, PFN_vkDebugUtilsMessengerCallbackEXT userCallback) {

			VkDebugUtilsMessageSeverityFlagsEXT messageSeverities;
			messageSeverities = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			#ifdef LF_BE_OVERLYSPECIFIC
			messageSeverities |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
			#endif

			createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createInfo.messageSeverity = messageSeverities;
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createInfo.pfnUserCallback = userCallback;
			createInfo.pUserData = nullptr;
		}

		// Extensions -----------------------------------------------------------------------------------
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

		std::vector<VkExtensionProperties> GetVulkanPhysicalDeviceExtensions(VkPhysicalDevice device) {

			uint32_t extensionCount;
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());
			return extensions;
		}

		std::vector<const char*> GetRequiredVulkanExtensions() {

			std::vector<const char*> extensions = GetRequiredGLFWExtensions();
			#ifdef LF_USE_VULKAN_VALIDATION_LAYERS
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			#endif
			return extensions;
		}

		// Layers ---------------------------------------------------------------------------------------
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

				for (const VkLayerProperties& availableLayerProperties : availableLayers) {

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

		// Queue Families -------------------------------------------------------------------------------
		std::vector<VkQueueFamilyProperties> GetVulkanQueueFamilies(VkPhysicalDevice device) {

			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			return queueFamilies;
		}

		QueueFamilyIndices IdentifyVulkanQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface) {

			QueueFamilyIndices indices;

			std::vector<VkQueueFamilyProperties> queueFamilies = GetVulkanQueueFamilies(device);

			for (int i = 0; i < queueFamilies.size(); i++) {

				if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					indices.HasGraphicsFamily = true;
					indices.GraphicsFamilyIndex = i;
				}
				VkBool32 presentSupport;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
				if (presentSupport) {
					indices.HasPresentFamily = true;
					indices.GraphicsFamilyIndex = i;
				}
				if (indices.IsComplete()) break;
			}

			return indices;
		}

		// SwapChain ------------------------------------------------------------------------------------
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

			for (const auto& availableFormat : availableFormats) {
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) // = Ideal format
					return availableFormat;
			}
			return availableFormats[0]; // We couldn't find our preferred format, so we will pick the first one.
		}

		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

			for (const auto& availablePresentMode : availablePresentModes) {
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) // Ideal present mode (renders as fast as possible, when queue is full first image is popped -> less latency, higher power usage)
					return availablePresentMode;
			}

			return VK_PRESENT_MODE_FIFO_KHR; // Is always available
		}

		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const Window& window) {

			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
				return capabilities.currentExtent;
			}
			else {

				int width, height;
				window.GetFramebufferSize(&width, &height);

				VkExtent2D actualExtent = {
					static_cast<uint32_t>(width),
					static_cast<uint32_t>(height)
				};

				actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

				return actualExtent;
			}
		}

		SwapChainSupportDetails GetSwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface) {

			SwapChainSupportDetails details;

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.Capabilities);

			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
			if (formatCount != 0) {
				details.Formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.Formats.data());
			}

			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
			if (presentModeCount != 0) {
				details.PresentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.PresentModes.data());
			}
			return details;
		}

		// PhysicalDevices ------------------------------------------------------------------------
		std::vector<VkPhysicalDevice> GetVulkanPhysicalDevices(VkInstance instance) {

			uint32_t physicalDeviceCount;
			vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
			LF_CORE_ASSERT(physicalDeviceCount, "Found no GPU's with Vulkan support!");
			std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
			vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());
			return physicalDevices;
		}

		void ListVulkanPhysicalDevices(VkInstance instance) {

			std::vector<VkPhysicalDevice> devices = GetVulkanPhysicalDevices(instance);

			std::string message = "All available Vulkan Physical Devices:\n";
			for (const VkPhysicalDevice& device : devices) {

				VkPhysicalDeviceProperties deviceProps;
				vkGetPhysicalDeviceProperties(device, &deviceProps);
				message += "\t" + (std::string)deviceProps.deviceName + "\n";
			}
			LF_CORE_INFO(message);
		}

		bool IsVulkanPhysicalDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface) {

			/* Don't need this yet
			VkPhysicalDeviceProperties deviceProps;
			vkGetPhysicalDeviceProperties(device, &deviceProps);
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
			*/

			QueueFamilyIndices indices = IdentifyVulkanQueueFamilies(device, surface);

			std::vector<VkExtensionProperties> availableExtensions = GetVulkanPhysicalDeviceExtensions(device);
			std::set<std::string> requiredDeviceExtensionsLeft(requiredVulkanDeviceExtensions.begin(), requiredVulkanDeviceExtensions.end());
			for (const VkExtensionProperties& extension : availableExtensions) {

				requiredDeviceExtensionsLeft.erase(extension.extensionName);
				if (requiredDeviceExtensionsLeft.empty()) break;
			}
			bool supportsRequiredExtensions = requiredDeviceExtensionsLeft.empty();
			bool swapChainIsAdequate = false;
			if (supportsRequiredExtensions) {
				SwapChainSupportDetails supportDetails = GetSwapChainSupportDetails(device, surface);
				swapChainIsAdequate = !(supportDetails.Formats.empty()) && !(supportDetails.PresentModes.empty());
			}

			return indices.IsComplete() && supportsRequiredExtensions && swapChainIsAdequate;
		}

		VkPhysicalDevice PickVulkanPhysicalDevice(VkInstance instance, VkSurfaceKHR surface) {

			std::vector<VkPhysicalDevice> physicalDevices = GetVulkanPhysicalDevices(instance);

			// Selects first suitable physical device
			VkPhysicalDevice physicalDevice = nullptr;
			for (auto device : physicalDevices) {
				if (IsVulkanPhysicalDeviceSuitable(device, surface)) {

					physicalDevice = device;
					break;
				}
			}

			LF_CORE_ASSERT(physicalDevice, "Failed to find suitable GPU!");
			return physicalDevice;
		}

		// Memory ---------------------------------------------------------------------------------------
		uint32_t FindMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties) {

			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
					return i;
				}
			}

			LF_CORE_ASSERT(false, "Failed to find suitable memory type!");
		}
	}
}