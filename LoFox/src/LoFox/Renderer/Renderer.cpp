#include "lfpch.h"
#include "LoFox/Renderer/Renderer.h"

#include "LoFox/Core/Application.h"

#include "LoFox/Renderer/SwapChain.h"
#include "LoFox/Renderer/Buffer.h"

#include "LoFox/Renderer/RenderCommand.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "LoFox/Renderer/RenderContext.h"

struct UniformBufferObject {

	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

namespace LoFox {

	Ref<Window> Renderer::m_Window;
	// Ref<SwapChain> Renderer::m_SwapChain;

	void Renderer::Init(Ref<Window> window) {

		m_Window = window;
		m_Timer.Reset();

		RenderCommand::Init();

		// m_SwapChain = CreateRef<SwapChain>(m_Window);

		// VkPhysicalDeviceProperties properties = {};
		// vkGetPhysicalDeviceProperties(VulkanContext::PhysicalDevice, &properties);
		// 
		// VkSamplerCreateInfo samplerCreateInfo = {};
		// samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		// samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		// samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		// samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		// samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		// samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		// samplerCreateInfo.anisotropyEnable = VK_TRUE;
		// samplerCreateInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		// samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK; // Unused: see addressModes
		// samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
		// samplerCreateInfo.compareEnable = VK_FALSE;
		// samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		// samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		// samplerCreateInfo.mipLodBias = 0.0f;
		// samplerCreateInfo.minLod = 0.0f;
		// samplerCreateInfo.maxLod = 0.0f;
		// 
		// LF_CORE_ASSERT(vkCreateSampler(VulkanContext::LogicalDevice, &samplerCreateInfo, nullptr, &m_Sampler) == VK_SUCCESS, "Failed to create image sampler!");
	}

	// void Renderer::SetResourceLayout(Ref<ResourceLayout> layout) {
	// 
	// 	m_ResourceLayout = layout;
	// 
	// 	CreateDescriptorPool();
	// 
	// 	// Allocate Descriptor sets
	// 	std::vector<VkDescriptorSetLayout> layouts(MaxFramesInFlight, m_ResourceLayout->GetDescriptorSetLayout());
	// 	VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
	// 	descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	// 	descriptorSetAllocInfo.descriptorPool = m_DescriptorPool;
	// 	descriptorSetAllocInfo.descriptorSetCount = (uint32_t)(MaxFramesInFlight);
	// 	descriptorSetAllocInfo.pSetLayouts = layouts.data();
	// 
	// 	m_GraphicsDescriptorSets.resize(MaxFramesInFlight);
	// 	LF_CORE_ASSERT(vkAllocateDescriptorSets(VulkanContext::LogicalDevice, &descriptorSetAllocInfo, m_GraphicsDescriptorSets.data()) == VK_SUCCESS, "Failed to allocate descriptor sets!");
	// 
	// 	// Update Descriptor sets
	// 	for (size_t i = 0; i < MaxFramesInFlight; i++) {
	// 
	// 		std::vector<VkWriteDescriptorSet> descriptorWrites = {};
	// 		uint32_t binding = 0;
	// 		for (const auto& resource : m_ResourceLayout->GetResources()) {
	// 
	// 			VkDescriptorBufferInfo bufferInfo = {};
	// 			if (resource.BufferDescriptorInfos.size() != 0)
	// 				bufferInfo = resource.BufferDescriptorInfos[i];
	// 
	// 			std::vector<VkDescriptorImageInfo> imageInfo = {};
	// 			if (resource.ImageDescriptorInfos.size() != 0)
	// 				imageInfo = resource.ImageDescriptorInfos;
	// 
	// 			VkWriteDescriptorSet descriptorWrite = {};
	// 			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	// 			descriptorWrite.dstSet = m_GraphicsDescriptorSets[i];
	// 			descriptorWrite.dstBinding = binding;
	// 			descriptorWrite.dstArrayElement = 0;
	// 			descriptorWrite.descriptorType = resource.Type;
	// 			descriptorWrite.descriptorCount = resource.DescriptorCount;
	// 			descriptorWrite.pBufferInfo = (resource.BufferDescriptorInfos.size() == 0) ? nullptr : &bufferInfo;
	// 			descriptorWrite.pImageInfo = (resource.ImageDescriptorInfos.size() == 0) ? nullptr : imageInfo.data();
	// 			descriptorWrite.pTexelBufferView = nullptr;
	// 
	// 			descriptorWrites.push_back(descriptorWrite);
	// 			binding++;
	// 
	// 			vkUpdateDescriptorSets(VulkanContext::LogicalDevice, 1, &descriptorWrite, 0, nullptr);
	// 		}
	// 	}
	// }

	// void Renderer::SubmitGraphicsPipeline(Ref<GraphicsPipeline> pipeline) {
	// 
	// 	m_GraphicsPipeline = pipeline;
	// 
	// 	// Important! The swapchain framebuffers need to be created after the pipeline is created.
	// 	m_SwapChain->CreateFramebuffers(m_GraphicsPipeline->RenderPass);
	// }
	
	void Renderer::Shutdown() {

		// vkDestroySampler(VulkanContext::LogicalDevice, m_Sampler, nullptr);
		// 
		// vkDestroyDescriptorPool(VulkanContext::LogicalDevice, m_DescriptorPool, nullptr);

		// m_SwapChain->Destroy();
		// m_GraphicsPipeline->Destroy();

		RenderCommand::Shutdown();
	}

	// void Renderer::PrepareFrame() {
	// 
	// 	m_SwapChain->BeginFrame();
	// 
	// 	VkCommandBuffer commandBuffer = m_SwapChain->GetThisFramesCommandbuffer();
	// 
	// 	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	// 	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	// 	commandBufferBeginInfo.flags = 0;
	// 	commandBufferBeginInfo.pInheritanceInfo = nullptr;
	// 
	// 	LF_CORE_ASSERT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS, "Failed to begin recording command buffer!");
	// }

	// void Renderer::StartFrame() {
	// 
	// 	VkCommandBuffer commandBuffer = m_SwapChain->GetThisFramesCommandbuffer();
	// 
	// 	std::vector<VkClearValue> clearValues = RenderCommand::GetClearValues();
	// 
	// 	VkRenderPassBeginInfo renderPassBeginInfo = {};
	// 	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	// 	renderPassBeginInfo.renderPass = m_GraphicsPipeline->RenderPass;
	// 	renderPassBeginInfo.framebuffer = m_SwapChain->GetThisFramesFramebuffer();
	// 	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	// 	renderPassBeginInfo.renderArea.extent = m_SwapChain->GetExtent();
	// 	renderPassBeginInfo.clearValueCount = (uint32_t)clearValues.size();
	// 	renderPassBeginInfo.pClearValues = clearValues.data();
	// 
	// 	vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	// 	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->Pipeline);
	// }

	// void Renderer::SubmitFrame() {
	// 
	// 	if (m_SwapChain->GetCurrentFrame() < 0)
	// 		return;
	// 
	// 	vkCmdEndRenderPass(m_SwapChain->GetThisFramesCommandbuffer());
	// 
	// 	LF_CORE_ASSERT(vkEndCommandBuffer(m_SwapChain->GetThisFramesCommandbuffer()) == VK_SUCCESS, "Failed to record command buffer!");
	// 
	// 	m_SwapChain->SubmitFrame();
	// 	RenderCommand::EmptyVertexBuffers();
	// }

	// void Renderer::WaitIdle() { vkDeviceWaitIdle(VulkanContext::LogicalDevice); }

	// void Renderer::Draw(uint32_t instances) {
	// 
	// 	VkCommandBuffer commandBuffer = m_SwapChain->GetThisFramesCommandbuffer();
	// 
	// 	vkCmdBindVertexBuffers(commandBuffer, 0, 1, RenderCommand::GetVertexBuffers().data(), RenderCommand::GetVertexBufferOffsets().data());
	// 	vkCmdBindIndexBuffer(commandBuffer, RenderCommand::GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
	// 	
	// 	vkCmdSetViewport(commandBuffer, 0, 1, &RenderCommand::GetViewport());
	// 	vkCmdSetScissor(commandBuffer, 0, 1, &RenderCommand::GetScissor());
	// 
	// 	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->Layout, 0, 1, &m_GraphicsDescriptorSets[m_SwapChain->GetCurrentFrame()], 0, nullptr);
	// 
	// 	std::vector<VkPushConstantRange> pushConstants = m_GraphicsPipeline->GetPushConstants();
	// 	std::vector<const void*> pushConstantsData = m_GraphicsPipeline->GetPushConstantsData();
	// 	for (size_t i = 0; i < pushConstants.size(); i++) {
	// 
	// 		VkPushConstantRange pushConstant = pushConstants[i];
	// 		const void* data = pushConstantsData[i];
	// 		vkCmdPushConstants(commandBuffer, m_GraphicsPipeline->Layout, pushConstant.stageFlags, pushConstant.offset, pushConstant.size, data);
	// 	}
	// 
	// 	vkCmdDrawIndexed(commandBuffer, RenderCommand::GetNumberOfIndices(), instances, 0, 0, 0);
	// }

	// bool Renderer::OnFramebufferResize(FramebufferResizeEvent& event) {
	// 
	// 	if (m_Window->IsMinimized())
	// 		return true;
	// 	m_SwapChain->Recreate();
	// 	m_SwapChain->CreateFramebuffers(m_GraphicsPipeline->RenderPass);
	// 	Application::GetInstance().OnUpdate();
	// 	return true;
	// }

	// void Renderer::CreateDescriptorPool() {
	// 
	// 	std::vector<VkDescriptorPoolSize> descriptorPoolSizes = {};
	// 	for (const auto& resource : m_ResourceLayout->GetResources()) {
	// 
	// 		VkDescriptorPoolSize descriptorPoolSize = {};
	// 		descriptorPoolSize.type = resource.Type;
	// 		descriptorPoolSize.descriptorCount = (uint32_t)(MaxFramesInFlight);
	// 		descriptorPoolSizes.push_back(descriptorPoolSize);
	// 	}
	// 
	// 	VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
	// 	descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	// 	descriptorPoolCreateInfo.poolSizeCount = descriptorPoolSizes.size();
	// 	descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
	// 	descriptorPoolCreateInfo.maxSets = (uint32_t)(MaxFramesInFlight);
	// 
	// 	LF_CORE_ASSERT(vkCreateDescriptorPool(VulkanContext::LogicalDevice, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool) == VK_SUCCESS, "Failed to create descriptor pool!");
	// }

}