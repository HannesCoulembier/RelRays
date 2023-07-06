#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

namespace LoFox {

	class DebugMessenger {

	public:
		DebugMessenger();

		inline VkDebugUtilsMessengerEXT GetDebugMessenger() { return m_DebugMessenger; }

		void Shutdown();

		inline PFN_vkDebugUtilsMessengerCallbackEXT GetMessageCallback() { return MessageCallback; }

		static Ref<DebugMessenger> Create();
	public:
		static const std::vector<const char*> ValidationLayers;
	private:
		void Init();

		static VKAPI_ATTR VkBool32 VKAPI_CALL MessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	private:
		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
	};
}