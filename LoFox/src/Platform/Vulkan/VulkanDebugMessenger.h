#pragma once

#include <vulkan/vulkan.h>

namespace LoFox {

	class VulkanDebugMessenger {

	public:
		VulkanDebugMessenger();

		inline VkDebugUtilsMessengerEXT GetDebugMessenger() { return m_DebugMessenger; }

		void Shutdown();

		inline static PFN_vkDebugUtilsMessengerCallbackEXT GetMessageCallback() { return MessageCallback; }

		static Ref<VulkanDebugMessenger> Create();
	public:
		static const std::vector<const char*> ValidationLayers;
	private:
		void Init();

		static VKAPI_ATTR VkBool32 VKAPI_CALL MessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	private:
		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
	};
}