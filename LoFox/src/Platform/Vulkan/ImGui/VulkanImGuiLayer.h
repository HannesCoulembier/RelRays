#pragma once

#include "LoFox/ImGui/ImGuiLayer.h"

#include <backends/imgui_impl_vulkan.h>

#include "LoFox/Renderer/Framebuffer.h"
#include "LoFox/Events/ApplicationEvent.h"
#include "LoFox/Events/KeyEvent.h"
#include "LoFox/Events/MouseEvent.h"

namespace LoFox {

	class VulkanImGuiLayer : public ImGuiLayer {

	public:
		VulkanImGuiLayer() {}
		~VulkanImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		virtual void Begin() override;
		virtual void End() override;
	private:
		void RecreateFramebuffers();
	private:
		VkDescriptorPool m_ImGuiDescriptorPool;
		VkRenderPass m_ImGuiRenderPass;
		std::vector<VkCommandBuffer> m_ImGuiCommandBuffers;
		VkCommandPool m_ImGuiCommandPool;
		std::vector<Ref<Framebuffer>> m_ImGuiFramebuffers;
		VkFormat m_ImGuiSwapchainImageFormat;
	};
}
