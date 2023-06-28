#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

namespace LoFox {

	namespace Utils {

		// Structs --------------------------------------------------------------------------------------
		struct QueueFamilyIndices {

			bool HasGraphicsFamily = false;
			uint32_t GraphicsFamilyIndex = 0;

			bool IsComplete() { return HasGraphicsFamily; }
		};

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

		// Queue Families -------------------------------------------------------------------------------
		std::vector<VkQueueFamilyProperties> GetVulkanQueueFamilies(VkPhysicalDevice device) {

			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			return queueFamilies;
		}

		QueueFamilyIndices IdentifyVulkanQueueFamilies(VkPhysicalDevice device) {

			QueueFamilyIndices indices;

			std::vector<VkQueueFamilyProperties> queueFamilies = GetVulkanQueueFamilies(device);

			for (int i = 0; i < queueFamilies.size(); i++) {

				if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
					indices.HasGraphicsFamily = true;
					indices.GraphicsFamilyIndex = i;
				}
				if (indices.IsComplete()) break;
			}

			return indices;
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

		bool IsVulkanPhysicalDeviceSuitable(VkPhysicalDevice device) {

			/* Don't need this yet
			VkPhysicalDeviceProperties deviceProps;
			vkGetPhysicalDeviceProperties(device, &deviceProps);
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
			*/

			QueueFamilyIndices indices = IdentifyVulkanQueueFamilies(device);

			return indices.IsComplete(); // We need our GPU to have a graphicsfamily
		}

		VkPhysicalDevice PickVulkanPhysicalDevice(VkInstance instance) {

			std::vector<VkPhysicalDevice> physicalDevices = GetVulkanPhysicalDevices(instance);

			// Selects first suitable physical device
			VkPhysicalDevice physicalDevice = nullptr;
			for (auto device : physicalDevices) {
				if (IsVulkanPhysicalDeviceSuitable(device)) {

					physicalDevice = device;
					break;
				}
			}

			LF_CORE_ASSERT(physicalDevice, "Failed to find suitable GPU!");
			return physicalDevice;
		}
	}
}