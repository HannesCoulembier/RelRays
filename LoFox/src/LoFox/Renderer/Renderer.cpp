#include "lfpch.h"
#include "LoFox/Renderer/Renderer.h"

#include "LoFox/Core/Application.h"

#include "LoFox/Renderer/SwapChain.h"
#include "LoFox/Renderer/Buffer.h"

#include "LoFox/Renderer/RenderContext.h"

namespace LoFox {

	Ref<Window> Renderer::m_Window;

	int Renderer::m_CurrentFrame = 0;
	uint32_t Renderer::m_ThisFramesImageIndex;

	const std::vector<uint32_t> vertexIndices = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
	};

	void Renderer::Init(Ref<Window> window) {

		m_Window = window;

		RenderContext::Init(m_Window);
	}

	void Renderer::Shutdown() {

		RenderContext::Shutdown();
	}

	void Renderer::StartFrame() {

		vkWaitForFences(RenderContext::LogicalDevice, 1, &RenderContext::InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		m_ThisFramesImageIndex = RenderContext::GetSwapChain()->AcquireNextImageIndex(m_CurrentFrame);
		if (m_ThisFramesImageIndex < 0)
			return;

		vkResetFences(RenderContext::LogicalDevice, 1, &RenderContext::InFlightFences[m_CurrentFrame]);

		vkResetCommandBuffer(RenderContext::CommandBuffers[m_CurrentFrame], 0);
	}

	void Renderer::SubmitFrame() {

		if (m_ThisFramesImageIndex < 0)
			return;

		RecordCommandBuffer(RenderContext::CommandBuffers[m_CurrentFrame]);
		RenderContext::UpdateUniformBuffer(m_CurrentFrame);

		VkSemaphore waitSemaphores[] = { RenderContext::ImageAvailableSemaphores[m_CurrentFrame] };
		VkSemaphore signalSemaphores[] = { RenderContext::RenderFinishedSemaphores[m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &RenderContext::CommandBuffers[m_CurrentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		LF_CORE_ASSERT(vkQueueSubmit(RenderContext::GraphicsQueueHandle, 1, &submitInfo, RenderContext::InFlightFences[m_CurrentFrame]) == VK_SUCCESS, "Failed to submit draw command buffer!");

		RenderContext::GetSwapChain()->PresentImage(m_ThisFramesImageIndex, signalSemaphores);

		m_CurrentFrame = (m_CurrentFrame + 1) % m_MaxFramesInFlight;
	}

	void Renderer::WaitIdle() { vkDeviceWaitIdle(RenderContext::LogicalDevice); }

	void Renderer::RecordCommandBuffer(VkCommandBuffer commandBuffer) {

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		LF_CORE_ASSERT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS, "Failed to begin recording command buffer!");

		std::array<VkClearValue, 2> clearColors = {};
		clearColors[0] = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearColors[1] = { 1.0f, 0 };
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = RenderContext::GetGraphicsPipeline().RenderPass;
		renderPassBeginInfo.framebuffer = RenderContext::GetSwapChain()->GetFramebuffer(m_ThisFramesImageIndex);
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = RenderContext::GetSwapChain()->GetExtent();
		renderPassBeginInfo.clearValueCount = (uint32_t)clearColors.size();
		renderPassBeginInfo.pClearValues = clearColors.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, RenderContext::GetGraphicsPipeline().Pipeline);

		VkBuffer vertexBuffers[] = { RenderContext::VertexBuffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commandBuffer, RenderContext::IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(RenderContext::GetSwapChain()->GetExtent().width);
		viewport.height = static_cast<float>(RenderContext::GetSwapChain()->GetExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = RenderContext::GetSwapChain()->GetExtent();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, RenderContext::GetGraphicsPipeline().Layout, 0, 1, &RenderContext::DescriptorSets[m_CurrentFrame], 0, nullptr);

		vkCmdDrawIndexed(commandBuffer, (uint32_t)vertexIndices.size(), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		LF_CORE_ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS, "Failed to record command buffer!");
	}

	bool Renderer::OnFramebufferResize(FramebufferResizeEvent& event) {

		RenderContext::GetSwapChain()->Recreate();
		Application::GetInstance().OnUpdate();
		return true;
	}
}