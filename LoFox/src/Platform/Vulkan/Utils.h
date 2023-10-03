#pragma once

#include <vulkan/vulkan.h>

namespace LoFox {

	namespace Utils {

		// Structs and stuff ----------------------------------------------------------------------------
		struct QueueFamilyIndices {

			bool HasGraphicsFamily = false;
			uint32_t GraphicsFamilyIndex = 0;
			bool HasPresentFamily = false;
			uint32_t PresentFamilyIndex = 0;
			bool GraphicsIsAlsoComputeFamily = false;

			bool IsComplete() { return HasGraphicsFamily && HasPresentFamily && GraphicsIsAlsoComputeFamily; }
		};

		struct SwapChainSupportDetails {

			VkSurfaceCapabilitiesKHR Capabilities = {};
			std::vector<VkSurfaceFormatKHR> Formats;
			std::vector<VkPresentModeKHR> PresentModes;
		};

		const std::vector<const char*> requiredVulkanDeviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		// Populators (populate createInfo structs) -----------------------------------------------------

		VkDebugUtilsMessengerCreateInfoEXT MakeDebugMessageCreateInfo(PFN_vkDebugUtilsMessengerCallbackEXT userCallback);

		// Extensions -----------------------------------------------------------------------------------
		std::vector<VkExtensionProperties> GetVulkanExtensions();

		void ListVulkanExtensions();

		std::vector<VkExtensionProperties> GetVulkanPhysicalDeviceExtensions(VkPhysicalDevice device);

		std::vector<const char*> GetRequiredVulkanExtensions();

		std::vector<const char*> GetRequiredGLFWExtensions();

		// Layers ---------------------------------------------------------------------------------------
		std::vector<VkLayerProperties> GetAvailableVulkanLayers();

		void ListAvailableVulkanLayers();

		// Checks if a list of layer names are available
		bool CheckVulkanValidationLayerSupport(const std::vector<const char*>& layers);

		// PhysicalDevices ------------------------------------------------------------------------

		std::vector<VkPhysicalDevice> GetVulkanPhysicalDevices(VkInstance instance);

		void ListVulkanPhysicalDevices(VkInstance instance);

		bool IsVulkanPhysicalDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);

		VkPhysicalDevice PickVulkanPhysicalDevice(VkInstance instance, VkSurfaceKHR surface);

		// Queue Families -------------------------------------------------------------------------------
		std::vector<VkQueueFamilyProperties> GetVulkanQueueFamilies(VkPhysicalDevice device);

		QueueFamilyIndices IdentifyVulkanQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

		// Swapchain ------------------------------------------------------------------------------------
		VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height);

		SwapChainSupportDetails GetSwapchainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface);

		// Memory ---------------------------------------------------------------------------------------
		uint32_t FindMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties);

		// Formats --------------------------------------------------------------------------------------
		VkFormat FindSupportedFormat(VkPhysicalDevice device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
		VkFormat FindDepthFormat(VkPhysicalDevice device);

		// Images ---------------------------------------------------------------------------------------
		VkImageView CreateImageViewFromImage(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	}
}