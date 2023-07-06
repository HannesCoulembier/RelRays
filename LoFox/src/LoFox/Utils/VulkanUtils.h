#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

#include "LoFox/Core/Window.h"

namespace LoFox {

	namespace Utils {

		// Structs and stuff ----------------------------------------------------------------------------
		struct QueueFamilyIndices {

			bool HasGraphicsFamily = false;
			uint32_t GraphicsFamilyIndex = 0;
			bool HasPresentFamily = false;
			uint32_t PresentFamilyIndex = 0;

			bool IsComplete() { return HasGraphicsFamily && HasPresentFamily; }
		};

		struct SwapChainSupportDetails {

			VkSurfaceCapabilitiesKHR Capabilities = {};
			std::vector<VkSurfaceFormatKHR> Formats;
			std::vector<VkPresentModeKHR> PresentModes;
		};

		const std::vector<const char*> requiredVulkanDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		// Populators (populate createInfo structs) -----------------------------------------------------
		void PopulateDebugMessageCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo, PFN_vkDebugUtilsMessengerCallbackEXT userCallback);

		// Extensions -----------------------------------------------------------------------------------
		std::vector<VkExtensionProperties> GetVulkanExtensions();

		void ListVulkanExtensions();

		std::vector<VkExtensionProperties> GetVulkanPhysicalDeviceExtensions(VkPhysicalDevice device);

		std::vector<const char*> GetRequiredVulkanExtensions();

		// Layers ---------------------------------------------------------------------------------------
		std::vector<VkLayerProperties> GetAvailableVulkanLayers();

		void ListAvailableVulkanLayers();

		// Checks if a list of layer names are available
		bool CheckVulkanValidationLayerSupport(const std::vector<const char*>& layers);

		// Queue Families -------------------------------------------------------------------------------
		std::vector<VkQueueFamilyProperties> GetVulkanQueueFamilies(VkPhysicalDevice device);

		QueueFamilyIndices IdentifyVulkanQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

		// SwapChain ------------------------------------------------------------------------------------
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, const Window& window);

		SwapChainSupportDetails GetSwapChainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface);

		// PhysicalDevices ------------------------------------------------------------------------
		std::vector<VkPhysicalDevice> GetVulkanPhysicalDevices(VkInstance instance);

		void ListVulkanPhysicalDevices(VkInstance instance);

		bool IsVulkanPhysicalDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

		VkPhysicalDevice PickVulkanPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);

		// Memory ---------------------------------------------------------------------------------------
		uint32_t FindMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties);

		// Formats --------------------------------------------------------------------------------------
		VkFormat FindSupportedFormat(VkPhysicalDevice device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat FindDepthFormat(VkPhysicalDevice device);
	
	}
}