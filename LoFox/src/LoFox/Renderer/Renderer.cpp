#include "lfpch.h"
#include "LoFox/Renderer/Renderer.h"

#include "LoFox/Core/Application.h"

#include "LoFox/Renderer/SwapChain.h"
#include "LoFox/Renderer/Buffer.h"

#include "LoFox/Renderer/RenderCommand.h"
#include "LoFox/Renderer/RenderContext.h"

struct UniformBufferObject {

	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

namespace LoFox {

	Ref<Window> Renderer::m_Window;
	Ref<SwapChain> Renderer::m_SwapChain;

	void Renderer::Init(Ref<Window> window) {

		m_Window = window;
		m_Timer.Reset();

		RenderContext::Init(m_Window);
		RenderCommand::Init();

		m_SwapChain = CreateRef<SwapChain>(m_Window);

		// Create GraphicsDescriptorset layouts
		VkDescriptorSetLayoutBinding uboLayoutBinding = {}; // uniform buffer with MVP matrices
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = nullptr;

		VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::vector<VkDescriptorSetLayoutBinding> graphicsBindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo graphicsDescriptorSetlayoutCreateInfo = {};
		graphicsDescriptorSetlayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		graphicsDescriptorSetlayoutCreateInfo.bindingCount = (uint32_t)graphicsBindings.size();
		graphicsDescriptorSetlayoutCreateInfo.pBindings = graphicsBindings.data();
		
		LF_CORE_ASSERT(vkCreateDescriptorSetLayout(RenderContext::LogicalDevice, &graphicsDescriptorSetlayoutCreateInfo, nullptr, &m_GraphicsDescriptorSetLayout) == VK_SUCCESS, "Failed to create descriptor set layout!");

		// Create UniformBuffers
		uint32_t uniformBufferSize = sizeof(UniformBufferObject);
		m_UniformBuffers.resize(MaxFramesInFlight);
		m_UniformBuffersMapped.resize(MaxFramesInFlight);

		for (size_t i = 0; i < MaxFramesInFlight; i++) {

			m_UniformBuffers[i] = CreateRef<Buffer>(uniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			vkMapMemory(RenderContext::LogicalDevice, m_UniformBuffers[i]->GetMemory(), 0, uniformBufferSize, 0, &m_UniformBuffersMapped[i]);
		}

		// Create textures
		m_Texture1 = CreateRef<Image>("Assets/Textures/Rick.png");
		
		CreateImageSampler();

		CreateDescriptorPool();

		// Allocate Descriptor sets
		std::vector<VkDescriptorSetLayout> layouts(MaxFramesInFlight, m_GraphicsDescriptorSetLayout);
		VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
		descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocInfo.descriptorPool = m_DescriptorPool;
		descriptorSetAllocInfo.descriptorSetCount = (uint32_t)(MaxFramesInFlight);
		descriptorSetAllocInfo.pSetLayouts = layouts.data();

		m_GraphicsDescriptorSets.resize(MaxFramesInFlight);
		LF_CORE_ASSERT(vkAllocateDescriptorSets(RenderContext::LogicalDevice, &descriptorSetAllocInfo, m_GraphicsDescriptorSets.data()) == VK_SUCCESS, "Failed to allocate descriptor sets!");

		// Update Descriptor sets
		for (size_t i = 0; i < MaxFramesInFlight; i++) {

			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = m_UniformBuffers[i]->GetBuffer();
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorImageInfo imageInfo = {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = m_Texture1->GetImageView();
			imageInfo.sampler = m_Sampler;

			std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].dstSet = m_GraphicsDescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;
			descriptorWrites[0].pImageInfo = nullptr;
			descriptorWrites[0].pTexelBufferView = nullptr;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_GraphicsDescriptorSets[i];
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = nullptr;
			descriptorWrites[1].pImageInfo = &imageInfo;
			descriptorWrites[1].pTexelBufferView = nullptr;

			vkUpdateDescriptorSets(RenderContext::LogicalDevice, (uint32_t)descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
		}
	}

	void Renderer::SubmitGraphicsPipeline(Ref<GraphicsPipeline> pipeline) {

		m_GraphicsPipeline = pipeline;

		// Important! The swapchain framebuffers need to be created after the pipeline is created.
		m_SwapChain->CreateFramebuffers(m_GraphicsPipeline->RenderPass);
	}
	
	void Renderer::Shutdown() {

		m_Texture1->Destroy();

		for (auto buffer : m_UniformBuffers)
			buffer->Destroy();

		vkDestroySampler(RenderContext::LogicalDevice, m_Sampler, nullptr);

		vkDestroyDescriptorPool(RenderContext::LogicalDevice, m_DescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(RenderContext::LogicalDevice, m_GraphicsDescriptorSetLayout, nullptr);

		m_SwapChain->Destroy();
		m_GraphicsPipeline->Destroy();

		RenderCommand::Shutdown();
		RenderContext::Shutdown();
	}

	void Renderer::StartFrame() {

		m_SwapChain->BeginFrame();

		VkCommandBuffer commandBuffer = m_SwapChain->GetThisFramesCommandbuffer();

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = 0;
		commandBufferBeginInfo.pInheritanceInfo = nullptr;

		LF_CORE_ASSERT(vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo) == VK_SUCCESS, "Failed to begin recording command buffer!");

		std::vector<VkClearValue> clearValues = RenderCommand::GetClearValues();

		VkRenderPassBeginInfo renderPassBeginInfo = {};
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.renderPass = m_GraphicsPipeline->RenderPass;
		renderPassBeginInfo.framebuffer = m_SwapChain->GetThisFramesFramebuffer();
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = m_SwapChain->GetExtent();
		renderPassBeginInfo.clearValueCount = (uint32_t)clearValues.size();
		renderPassBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->Pipeline);
	}

	void Renderer::SubmitFrame() {

		if (m_SwapChain->GetCurrentFrame() < 0)
			return;

		// To be moved to client ----------------------------------------------------
		UpdateUniformBuffer(m_SwapChain->GetCurrentFrame());
		// To be moved to client ----------------------------------------------------

		RecordCommandBuffer();

		vkCmdEndRenderPass(m_SwapChain->GetThisFramesCommandbuffer());

		LF_CORE_ASSERT(vkEndCommandBuffer(m_SwapChain->GetThisFramesCommandbuffer()) == VK_SUCCESS, "Failed to record command buffer!");

		m_SwapChain->SubmitFrame();
		RenderCommand::EmptyVertexBuffers();
	}

	void Renderer::WaitIdle() { vkDeviceWaitIdle(RenderContext::LogicalDevice); }

	void Renderer::RecordCommandBuffer() {

		VkCommandBuffer commandBuffer = m_SwapChain->GetThisFramesCommandbuffer();

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, RenderCommand::GetVertexBuffers().data(), RenderCommand::GetVertexBufferOffsets().data());
		vkCmdBindIndexBuffer(commandBuffer, RenderCommand::GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
		
		vkCmdSetViewport(commandBuffer, 0, 1, &RenderCommand::GetViewport());
		vkCmdSetScissor(commandBuffer, 0, 1, &RenderCommand::GetScissor());

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->Layout, 0, 1, &m_GraphicsDescriptorSets[m_SwapChain->GetCurrentFrame()], 0, nullptr);

		std::vector<VkPushConstantRange> pushConstants = m_GraphicsPipeline->GetPushConstants();
		std::vector<const void*> pushConstantsData = m_GraphicsPipeline->GetPushConstantsData();
		for (size_t i = 0; i < pushConstants.size(); i++) {

			VkPushConstantRange pushConstant = pushConstants[i];
			const void* data = pushConstantsData[i];
			vkCmdPushConstants(commandBuffer, m_GraphicsPipeline->Layout, pushConstant.stageFlags, pushConstant.offset, pushConstant.size, data);
		}

		vkCmdDrawIndexed(commandBuffer, RenderCommand::GetNumberOfIndices(), 1, 0, 0, 0);
	}

	void Renderer::UpdateUniformBuffer(uint32_t currentImage) {

		float time = m_Timer.Elapsed();

		UniformBufferObject ubo = {};
		ubo.view = glm::lookAt(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), m_SwapChain->GetExtent().width / (float)m_SwapChain->GetExtent().height, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1; // glm was designed for OpenGL, where the y-axis is flipped. This unflips it for Vulkan

		memcpy(m_UniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}

	bool Renderer::OnFramebufferResize(FramebufferResizeEvent& event) {

		if (m_Window->IsMinimized())
			return true;
		m_SwapChain->Recreate();
		m_SwapChain->CreateFramebuffers(m_GraphicsPipeline->RenderPass);
		Application::GetInstance().OnUpdate();
		return true;
	}

	void Renderer::CreateImageSampler() {

		VkPhysicalDeviceProperties properties = {};
		vkGetPhysicalDeviceProperties(RenderContext::PhysicalDevice, &properties);

		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.anisotropyEnable = VK_TRUE;
		samplerCreateInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_TRANSPARENT_BLACK; // Unused: see addressModes
		samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;
		samplerCreateInfo.compareEnable = VK_FALSE;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod = 0.0f;

		LF_CORE_ASSERT(vkCreateSampler(RenderContext::LogicalDevice, &samplerCreateInfo, nullptr, &m_Sampler) == VK_SUCCESS, "Failed to create image sampler!");
	}

	void Renderer::CreateDescriptorPool() {

		std::vector<VkDescriptorPoolSize> descriptorPoolSizes(2);
		descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorPoolSizes[0].descriptorCount = (uint32_t)(MaxFramesInFlight);
		descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorPoolSizes[1].descriptorCount = (uint32_t)(MaxFramesInFlight);

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.poolSizeCount = 2;
		descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
		descriptorPoolCreateInfo.maxSets = (uint32_t)(MaxFramesInFlight);

		LF_CORE_ASSERT(vkCreateDescriptorPool(RenderContext::LogicalDevice, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool) == VK_SUCCESS, "Failed to create descriptor pool!");
	}

}