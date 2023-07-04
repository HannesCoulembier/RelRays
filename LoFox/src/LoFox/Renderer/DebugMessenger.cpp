#include "lfpch.h"
#include "LoFox/Renderer/DebugMessenger.h"

#include "LoFox/Utils/VulkanUtils.h"

namespace LoFox {

	VkResult CreateVulkanDebugMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {

		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func == nullptr)
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}

	void DestroyVulkanDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {

		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(instance, debugMessenger, pAllocator);
	}
	
	VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessenger::MessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {

		#ifndef LF_BE_OVERLYSPECIFIC
		if (messageSeverity < VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			return VK_FALSE;
		#endif

		switch (messageSeverity) {

		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: { LF_CORE_INFO("Vulkan [INFO]: " + (std::string)pCallbackData->pMessage); break; }
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: { LF_CORE_INFO("Vulkan [VERBOSE]: " + (std::string)pCallbackData->pMessage); break; }
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: { LF_CORE_WARN("Vulkan [WARNING]: " + (std::string)pCallbackData->pMessage); break; }
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: { LF_CORE_ERROR("Vulkan [ERROR]: " + (std::string)pCallbackData->pMessage); break; }
		}

		return VK_FALSE; // When VK_TRUE is returned, Vulkan will abort the call that made this callback
	}

	DebugMessenger::DebugMessenger(Ref<RenderContext> context)
		: m_Context(context) {

		#ifdef LF_USE_VULKAN_VALIDATION_LAYERS

		std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

		LF_CORE_ASSERT(Utils::CheckVulkanValidationLayerSupport(m_ValidationLayers), "Validation layers requested, but not available!");
		
		#else

		std::vector<const char*> m_ValidationLayers = {};

		#endif
	}

	void DebugMessenger::Init() {

		#ifdef LF_USE_VULKAN_VALIDATION_LAYERS

		VkDebugUtilsMessageSeverityFlagsEXT messageSeverities;
		messageSeverities = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		#ifdef LF_BE_OVERLYSPECIFIC
		messageSeverities |= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
		#endif

		VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo = {};
		messengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		messengerCreateInfo.messageSeverity = messageSeverities;
		messengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		messengerCreateInfo.pfnUserCallback = MessageCallback;
		messengerCreateInfo.pUserData = nullptr;

		LF_CORE_ASSERT(CreateVulkanDebugMessengerEXT(m_Context->Instance, &messengerCreateInfo, nullptr, &m_DebugMessenger) == VK_SUCCESS, "Failed to set up debug messenger!");
		
		#endif
	}

	void DebugMessenger::Shutdown() {

		#ifdef LF_USE_VULKAN_VALIDATION_LAYERS
		
		DestroyVulkanDebugUtilsMessengerEXT(m_Context->Instance, m_DebugMessenger, nullptr);
		
		#endif
	}

	Ref<DebugMessenger> DebugMessenger::Create(Ref<RenderContext> context) {

		return CreateRef<DebugMessenger>(context);
	}
}