#pragma once

#include "LoFox/Core/Core.h"

#include "LoFox/Renderer/RenderContext.h"

namespace LoFox {

	class DebugMessenger {

	public:
		DebugMessenger(Ref<RenderContext> context);

		inline VkDebugUtilsMessengerEXT GetDebugMessenger() { return m_DebugMessenger; }

		void Shutdown();

		inline PFN_vkDebugUtilsMessengerCallbackEXT GetMessageCallback() { return MessageCallback; }

		static Ref<DebugMessenger> Create(Ref<RenderContext> context);
	public:
		static const std::vector<const char*> ValidationLayers;
	private:
		void Init();

		static VKAPI_ATTR VkBool32 VKAPI_CALL MessageCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	private:
		Ref<RenderContext> m_Context;

		VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
	};
}