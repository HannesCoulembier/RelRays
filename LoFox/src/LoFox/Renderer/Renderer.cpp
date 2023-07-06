#include "lfpch.h"
#include "LoFox/Renderer/Renderer.h"

#include "LoFox/Core/Application.h"

#include "LoFox/Renderer/SwapChain.h"
#include "LoFox/Renderer/Buffer.h"

#include "LoFox/Renderer/RenderContext.h"

struct Vertex {

	glm::vec3 Position;
	glm::vec3 Color;
	glm::vec2 TexCoord;

	static VkVertexInputBindingDescription GetBindingDescription() {

		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, Position);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, Color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, TexCoord);

		return attributeDescriptions;
	}
};

struct UniformBufferObject {

	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

	{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
	{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
	{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
	{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
};

const std::vector<uint32_t> vertexIndices = {
	0, 1, 2, 2, 3, 0,
	4, 5, 6, 6, 7, 4,
};

namespace LoFox {

	Ref<Window> Renderer::m_Window;
	Timer Renderer::m_Timer;

	Ref<Buffer> Renderer::m_VertexBuffer;
	Ref<Buffer> Renderer::m_IndexBuffer;

	VkSampler Renderer::m_Sampler;
	Ref<Image> Renderer::m_Texture1;

	std::vector<Ref<Buffer>> Renderer::m_UniformBuffers;
	std::vector<void*> Renderer::m_UniformBuffersMapped;

	VkDescriptorPool Renderer::m_DescriptorPool;
	VkDescriptorSetLayout Renderer::m_DescriptorSetLayout;
	std::vector<VkDescriptorSet> Renderer::m_DescriptorSets;

	int Renderer::m_CurrentFrame = 0;
	uint32_t Renderer::m_ThisFramesImageIndex;

	void Renderer::Init(Ref<Window> window) {

		m_Window = window;
		m_Timer.Reset();

		RenderContext::Init(m_Window);

		// Create Descriptorset layouts
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

		std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
		layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutCreateInfo.bindingCount = (uint32_t)bindings.size();
		layoutCreateInfo.pBindings = bindings.data();

		LF_CORE_ASSERT(vkCreateDescriptorSetLayout(RenderContext::LogicalDevice, &layoutCreateInfo, nullptr, &m_DescriptorSetLayout) == VK_SUCCESS, "Failed to create descriptor set layout!");

		RenderContext::InitPipelines(m_DescriptorSetLayout, Vertex::GetBindingDescription(), Vertex::GetAttributeDescriptions());

		// Create Vertex buffer
		uint32_t vertexBufferSize = sizeof(vertices[0]) * vertices.size();
		Ref<Buffer> vertexStagingBuffer = CreateRef<Buffer>(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_VertexBuffer = CreateRef<Buffer>(vertexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		vertexStagingBuffer->SetData(vertices.data());

		RenderContext::CopyBuffer(vertexStagingBuffer, m_VertexBuffer);
		vertexStagingBuffer->Destroy();

		// Create Index buffer
		uint32_t indexBufferSize = sizeof(vertexIndices[0]) * vertexIndices.size();
		Ref<Buffer> indexStagingBuffer = CreateRef<Buffer>(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		m_IndexBuffer = CreateRef<Buffer>(indexBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		indexStagingBuffer->SetData(vertexIndices.data());

		RenderContext::CopyBuffer(indexStagingBuffer, m_IndexBuffer);
		indexStagingBuffer->Destroy();

		// Create UniformBuffers
		uint32_t uniformBufferSize = sizeof(UniformBufferObject);
		m_UniformBuffers.resize(m_MaxFramesInFlight);
		m_UniformBuffersMapped.resize(m_MaxFramesInFlight);

		for (size_t i = 0; i < m_MaxFramesInFlight; i++) {

			m_UniformBuffers[i] = CreateRef<Buffer>(uniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

			vkMapMemory(RenderContext::LogicalDevice, m_UniformBuffers[i]->GetMemory(), 0, uniformBufferSize, 0, &m_UniformBuffersMapped[i]);
		}

		// Create textures
		m_Texture1 = CreateRef<Image>("Assets/Textures/Rick.png");

		CreateImageSampler();

		// Create Descriptor pool
		std::array<VkDescriptorPoolSize, 2> descriptorPoolSizes = {};
		descriptorPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorPoolSizes[0].descriptorCount = (uint32_t)(m_MaxFramesInFlight);
		descriptorPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorPoolSizes[1].descriptorCount = (uint32_t)(m_MaxFramesInFlight);

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo = {};
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.poolSizeCount = 2;
		descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();
		descriptorPoolCreateInfo.maxSets = (uint32_t)(m_MaxFramesInFlight);

		LF_CORE_ASSERT(vkCreateDescriptorPool(RenderContext::LogicalDevice, &descriptorPoolCreateInfo, nullptr, &m_DescriptorPool) == VK_SUCCESS, "Failed to create descriptor pool!");

		// Create Descriptor sets
		std::vector<VkDescriptorSetLayout> layouts(m_MaxFramesInFlight, m_DescriptorSetLayout);
		VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
		descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocInfo.descriptorPool = m_DescriptorPool;
		descriptorSetAllocInfo.descriptorSetCount = (uint32_t)(m_MaxFramesInFlight);
		descriptorSetAllocInfo.pSetLayouts = layouts.data();

		m_DescriptorSets.resize(m_MaxFramesInFlight);
		LF_CORE_ASSERT(vkAllocateDescriptorSets(RenderContext::LogicalDevice, &descriptorSetAllocInfo, m_DescriptorSets.data()) == VK_SUCCESS, "Failed to allocate descriptor sets!");

		for (size_t i = 0; i < m_MaxFramesInFlight; i++) {

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
			descriptorWrites[0].dstSet = m_DescriptorSets[i];
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;
			descriptorWrites[0].pImageInfo = nullptr;
			descriptorWrites[0].pTexelBufferView = nullptr;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].dstSet = m_DescriptorSets[i];
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

	void Renderer::Shutdown() {

		m_VertexBuffer->Destroy();
		m_IndexBuffer->Destroy();

		m_Texture1->Destroy();

		for (auto buffer : m_UniformBuffers)
			buffer->Destroy();

		vkDestroySampler(RenderContext::LogicalDevice, m_Sampler, nullptr);

		vkDestroyDescriptorPool(RenderContext::LogicalDevice, m_DescriptorPool, nullptr);
		vkDestroyDescriptorSetLayout(RenderContext::LogicalDevice, m_DescriptorSetLayout, nullptr);

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
		UpdateUniformBuffer(m_CurrentFrame);

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

		VkBuffer vertexBuffers[] = { m_VertexBuffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT32);

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

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, RenderContext::GetGraphicsPipeline().Layout, 0, 1, &m_DescriptorSets[m_CurrentFrame], 0, nullptr);

		vkCmdDrawIndexed(commandBuffer, (uint32_t)vertexIndices.size(), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		LF_CORE_ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS, "Failed to record command buffer!");
	}

	void Renderer::UpdateUniformBuffer(uint32_t currentImage) {

		float time = m_Timer.Elapsed();

		UniformBufferObject ubo = {};
		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), RenderContext::GetSwapChain()->GetExtent().width / (float)RenderContext::GetSwapChain()->GetExtent().height, 0.1f, 10.0f); // TODO: Fix error when minimizing window (SwapChainExtent.height becomes 0)
		ubo.proj[1][1] *= -1; // glm was designed for OpenGL, where the y-axis is flipped. This unflips it for Vulkan

		memcpy(m_UniformBuffersMapped[currentImage], &ubo, sizeof(ubo));
	}

	bool Renderer::OnFramebufferResize(FramebufferResizeEvent& event) {

		RenderContext::GetSwapChain()->Recreate();
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
}