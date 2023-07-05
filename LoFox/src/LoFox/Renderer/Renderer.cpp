#include "lfpch.h"
#include "LoFox/Renderer/Renderer.h"

#include "LoFox/Renderer/SwapChain.h"
#include "LoFox/Renderer/Buffer.h"

namespace LoFox {

	Ref<Window> Renderer::m_Window;
	Ref<RenderContext> Renderer::m_Context;

	int Renderer::m_CurrentFrame = 0;
	uint32_t Renderer::m_ThisFramesImageIndex;

	const std::vector<uint32_t> vertexIndices = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
	};

	void Renderer::Init(Ref<Window> window) {

		m_Window = window;

		m_Context = RenderContext::Create();
		m_Context->Init(m_Window);
	}

	void Renderer::Shutdown() {

		m_Context->Shutdown();
	}

	void Renderer::StartFrame() {

		vkWaitForFences(m_Context->LogicalDevice, 1, &m_Context->InFlightFences[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		m_ThisFramesImageIndex = m_Context->GetSwapChain()->AcquireNextImageIndex(m_CurrentFrame);
		if (m_ThisFramesImageIndex < 0)
			return;

		vkResetFences(m_Context->LogicalDevice, 1, &m_Context->InFlightFences[m_CurrentFrame]);

		vkResetCommandBuffer(m_Context->CommandBuffers[m_CurrentFrame], 0);
	}

	void Renderer::SubmitFrame() {

		if (m_ThisFramesImageIndex < 0)
			return;

		RecordCommandBuffer(m_Context->CommandBuffers[m_CurrentFrame]);
		m_Context->UpdateUniformBuffer(m_CurrentFrame);

		VkSemaphore waitSemaphores[] = { m_Context->ImageAvailableSemaphores[m_CurrentFrame] };
		VkSemaphore signalSemaphores[] = { m_Context->RenderFinishedSemaphores[m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_Context->CommandBuffers[m_CurrentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		LF_CORE_ASSERT(vkQueueSubmit(m_Context->GraphicsQueueHandle, 1, &submitInfo, m_Context->InFlightFences[m_CurrentFrame]) == VK_SUCCESS, "Failed to submit draw command buffer!");

		m_Context->GetSwapChain()->PresentImage(m_ThisFramesImageIndex, signalSemaphores);

		m_CurrentFrame = (m_CurrentFrame + 1) % m_MaxFramesInFlight;
	}

	void Renderer::RecordCommandBuffer(VkCommandBuffer commandBuffer) {

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		LF_CORE_ASSERT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS, "Failed to begin recording command buffer!");

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };
		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = m_Context->Renderpass;
		renderPassBeginInfo.framebuffer = m_Context->GetSwapChain()->GetFramebuffer(m_ThisFramesImageIndex);
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = m_Context->GetSwapChain()->GetExtent();
		renderPassBeginInfo.clearValueCount = 1;
		renderPassBeginInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Context->GraphicsPipeline);

		VkBuffer vertexBuffers[] = { m_Context->VertexBuffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commandBuffer, m_Context->IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_Context->GetSwapChain()->GetExtent().width);
		viewport.height = static_cast<float>(m_Context->GetSwapChain()->GetExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = m_Context->GetSwapChain()->GetExtent();
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Context->PipelineLayout, 0, 1, &m_Context->DescriptorSets[m_CurrentFrame], 0, nullptr);

		vkCmdDrawIndexed(commandBuffer, (uint32_t)vertexIndices.size(), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		LF_CORE_ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS, "Failed to record command buffer!");
	}
}