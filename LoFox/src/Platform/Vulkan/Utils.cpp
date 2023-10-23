#include "lfpch.h"
#include "Platform/Vulkan/Utils.h"

#include <GLFW/glfw3.h>

namespace LoFox {

	namespace Utils {

		// Populators (populate createInfo structs) -----------------------------------------------------
		VkDebugUtilsMessengerCreateInfoEXT MakeDebugMessageCreateInfo(PFN_vkDebugUtilsMessengerCallbackEXT userCallback) {

			VkDebugUtilsMessengerCreateInfoEXT createInfo = {};

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
			return createInfo;
		}

		VkPipelineShaderStageCreateInfo MakePipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, const char* mainFuncName, VkShaderModule module) {

			VkPipelineShaderStageCreateInfo createInfo = {};
			createInfo.flags = 0;
			createInfo.module = module;
			createInfo.pName = mainFuncName;
			createInfo.pNext = nullptr;
			createInfo.pSpecializationInfo = nullptr;
			createInfo.stage = stage;
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			return createInfo;
		}

		VkPipelineVertexInputStateCreateInfo MakePipelineVertexInputStateCreateInfo(const std::vector<VkVertexInputBindingDescription>& bindings, const std::vector<VkVertexInputAttributeDescription>& attributes) {

			VkPipelineVertexInputStateCreateInfo createInfo = {};
			createInfo.flags = 0;
			createInfo.pNext = nullptr;
			createInfo.pVertexAttributeDescriptions = attributes.data();
			createInfo.pVertexBindingDescriptions = bindings.data();
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			createInfo.vertexAttributeDescriptionCount = attributes.size();
			createInfo.vertexBindingDescriptionCount = bindings.size();
			return createInfo;
		}

		VkPipelineInputAssemblyStateCreateInfo MakePipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology) {

			VkPipelineInputAssemblyStateCreateInfo createInfo = {};
			createInfo.flags = 0;
			createInfo.pNext = nullptr;
			createInfo.primitiveRestartEnable = VK_FALSE; // TODO: make more advanced when implementing more possible topologies (see: https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkPipelineInputAssemblyStateCreateInfo.html)
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			createInfo.topology = topology;
			return createInfo;
		}

		VkPipelineRasterizationStateCreateInfo MakePipelineRasterizationStateCreateInfo(VkPolygonMode polygonMode, float lineWidth) {

			VkPipelineRasterizationStateCreateInfo createInfo = {};
			createInfo.cullMode = VK_CULL_MODE_NONE; // TODO: add support for all cull modes (if equivalents exist in OpenGL) see: https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkCullModeFlagBits.html

			// TODO: investigate when adding depth testing?
			// For all of this depth stuff, see: https://github.com/KhronosGroup/Vulkan-Guide/blob/main/chapters/depth.adoc
			createInfo.depthBiasClamp = 0.0f;
			createInfo.depthBiasConstantFactor = 0.0f;
			createInfo.depthBiasEnable = VK_FALSE;
			createInfo.depthBiasSlopeFactor = 0.0f;
			createInfo.depthClampEnable = VK_FALSE;
			// End depth stuff

			createInfo.flags = 0;
			createInfo.frontFace = VK_FRONT_FACE_CLOCKWISE; // TODO: add support for both clockwise as anticlockwise, see createInfo.cullMode and: https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkFrontFace.html
			createInfo.lineWidth = lineWidth;
			createInfo.pNext = nullptr;
			createInfo.polygonMode = polygonMode;
			createInfo.rasterizerDiscardEnable = VK_FALSE; // Only usecases of disabling the rasterizer like this is are for véry niche cases. I won't bother
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			return createInfo;
		}

		VkPipelineMultisampleStateCreateInfo MakePipelineMultisampleStateCreateInfo() {

			VkPipelineMultisampleStateCreateInfo createInfo = {};
			createInfo.alphaToCoverageEnable = VK_FALSE;
			createInfo.alphaToOneEnable = VK_FALSE;
			createInfo.flags = 0;
			createInfo.minSampleShading = 1.0f;
			createInfo.pNext = nullptr;
			createInfo.pSampleMask = nullptr;
			createInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
			createInfo.sampleShadingEnable = VK_FALSE;
			createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			return createInfo;
		}

		VkPipelineColorBlendAttachmentState MakePipelineColorBlendAttachmentState() {

			VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
			// colorBlendAttachment.alphaBlendOp = 
			colorBlendAttachment.blendEnable = VK_FALSE;
			// colorBlendAttachment.colorBlendOp = 
			colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
			// colorBlendAttachment.dstAlphaBlendFactor = 
			// colorBlendAttachment.dstColorBlendFactor = 
			// colorBlendAttachment.srcAlphaBlendFactor = 
			// colorBlendAttachment.srcColorBlendFactor = 
			return colorBlendAttachment;
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

		// Returns the extensions glfw needs to operate
		std::vector<const char*> GetRequiredGLFWExtensions() {

			uint32_t glfwExtensionCount;
			const char** glfwExtensions;
			glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

			std::vector<const char*> requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
			return requiredExtensions;
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
			*/
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

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
				SwapChainSupportDetails supportDetails = GetSwapchainSupportDetails(device, surface);
				swapChainIsAdequate = !(supportDetails.Formats.empty()) && !(supportDetails.PresentModes.empty());
			}

			return indices.IsComplete() && supportsRequiredExtensions && swapChainIsAdequate && deviceFeatures.samplerAnisotropy;
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

					if (queueFamilies[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
						indices.GraphicsIsAlsoComputeFamily = true;
				}
				VkBool32 presentSupport;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
				if (presentSupport) {
					indices.HasPresentFamily = true;
					indices.PresentFamilyIndex = i;
				}
				if (indices.IsComplete()) break;
			}

			return indices;
		}

		// Swapchain ------------------------------------------------------------------------------------
		VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

			for (const auto& availableFormat : availableFormats) {
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) // = Ideal format
					return availableFormat;
			}
			return availableFormats[0]; // We couldn't find our preferred format, so we will pick the first one.
		}

		VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {

			for (const auto& availablePresentMode : availablePresentModes) {
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) // Ideal present mode (renders as fast as possible, when queue is full first image is popped -> less latency, higher power usage)
					return availablePresentMode;
			}

			return VK_PRESENT_MODE_FIFO_KHR; // Is always available
		}

		VkExtent2D ChooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t width, uint32_t height) {

			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
				return capabilities.currentExtent;
			}
			else {

				VkExtent2D actualExtent = {
					width,
					height
				};

				actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

				return actualExtent;
			}
		}

		SwapChainSupportDetails GetSwapchainSupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface) {

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

		// Formats --------------------------------------------------------------------------------------
		VkFormat FindSupportedFormat(VkPhysicalDevice device, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {

			for (VkFormat format : candidates) {
				VkFormatProperties props;
				vkGetPhysicalDeviceFormatProperties(device, format, &props);

				if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
					return format;
				else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
					return format;
			}

			LF_CORE_ASSERT(false, "Failed to find a supported format between candidates!");
		}

		VkFormat FindDepthFormat(VkPhysicalDevice device) { return FindSupportedFormat(device, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT); }
	
		// Images ---------------------------------------------------------------------------------------
		VkImageView CreateImageViewFromImage(VkDevice logicalDevice, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) {

			VkImageViewCreateInfo imageViewCreateInfo = {};
			imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			imageViewCreateInfo.image = image;
			imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			imageViewCreateInfo.format = format;
			imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
			imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
			imageViewCreateInfo.subresourceRange.levelCount = 1;
			imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
			imageViewCreateInfo.subresourceRange.layerCount = 1;

			VkImageView imageView;
			LF_CORE_ASSERT(vkCreateImageView(logicalDevice, &imageViewCreateInfo, nullptr, &imageView) == VK_SUCCESS, "Failed to create image view!");
			return imageView;
		}
	}
}