#include "lfpch.h"
#include "LoFox/Renderer/RenderContext.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE // glm is made for OpenGL that has depth range -1 to 1, we need 0 to 1
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "LoFox/Renderer/Shader.h"
#include "LoFox/Renderer/Buffer.h"
#include "LoFox/Renderer/SwapChain.h"
#include "LoFox/Renderer/Image.h"
#include "LoFox/Renderer/Pipeline.h"

#include "LoFox/Renderer/DebugMessenger.h"

#include "LoFox/Core/Settings.h"
#include "LoFox/Utils/VulkanUtils.h"
#include "LoFox/Utils/Utils.h"

#include "LoFox/Events/RenderEvent.h"

#include "LoFox/Renderer/Renderer.h"

#include "LoFox/Core/Application.h"

namespace LoFox {

	VkInstance RenderContext::Instance;
	VkPhysicalDevice RenderContext::PhysicalDevice;
	VkDevice RenderContext::LogicalDevice;
	VkSurfaceKHR RenderContext::Surface;

	std::vector<VkCommandBuffer> RenderContext::CommandBuffers;

	VkQueue RenderContext::GraphicsQueueHandle;
	VkQueue RenderContext::PresentQueueHandle;

	std::vector<VkSemaphore> RenderContext::ImageAvailableSemaphores;
	std::vector<VkSemaphore> RenderContext::RenderFinishedSemaphores;
	std::vector<VkFence> RenderContext::InFlightFences;

	Ref<DebugMessenger> RenderContext::m_DebugMessenger;
	Ref<Window> RenderContext::m_Window;

	Ref<SwapChain> RenderContext::m_SwapChain;
	GraphicsPipeline RenderContext::m_GraphicsPipeline;

	VkCommandPool RenderContext::m_CommandPool;

	void RenderContext::Init(Ref<Window> window) {

		m_Window = window;

		LF_OVERSPECIFY("Creating Vulkan instance");
		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanExtensions();
		Utils::ListAvailableVulkanLayers();
		#endif

		InitInstance();
		m_DebugMessenger = DebugMessenger::Create();

		// Setting up Surface
		m_Window->CreateVulkanSurface(Instance, &Surface);

		InitDevices();
		m_SwapChain = CreateRef<SwapChain>(m_Window);
		
		CreateSyncObjects();

		// Create Commandbuffers
		CommandBuffers.resize(m_MaxFramesInFlight);

		VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.commandPool = m_CommandPool;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.commandBufferCount = (uint32_t)CommandBuffers.size();

		LF_CORE_ASSERT(vkAllocateCommandBuffers(LogicalDevice, &commandBufferAllocInfo, CommandBuffers.data()) == VK_SUCCESS, "Failed to allocate command buffers!");
	}
	
	void RenderContext::Shutdown() {

		m_SwapChain->Destroy();

		for (size_t i = 0; i < m_MaxFramesInFlight; i++) {

			vkDestroySemaphore(LogicalDevice, ImageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(LogicalDevice, RenderFinishedSemaphores[i], nullptr);
			vkDestroyFence(LogicalDevice, InFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(LogicalDevice, m_CommandPool, nullptr);

		m_GraphicsPipeline.Destroy();

		m_DebugMessenger->Shutdown();

		vkDestroyDevice(LogicalDevice, nullptr);

		vkDestroySurfaceKHR(Instance, Surface, nullptr);

		vkDestroyInstance(Instance, nullptr);
	}

	void RenderContext::InitInstance() {

		VkApplicationInfo appInfo;
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = NULL;
		appInfo.pApplicationName = m_Window->GetSpec().Title.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(LF_VERSION_MAJOR, LF_VERSION_MINOR, LF_VERSION_PATCH);
		appInfo.pEngineName = LF_ENGINE_NAME;
		appInfo.engineVersion = VK_MAKE_VERSION(LF_VERSION_MAJOR, LF_VERSION_MINOR, LF_VERSION_PATCH);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		auto extensions = Utils::GetRequiredVulkanExtensions();
		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();
		instanceCreateInfo.enabledLayerCount = (uint32_t)(DebugMessenger::ValidationLayers.size());
		instanceCreateInfo.ppEnabledLayerNames = DebugMessenger::ValidationLayers.data();
		instanceCreateInfo.pNext = nullptr;

		#ifdef LF_BE_OVERLYSPECIFIC
		// Add debug messenger to instance creation and cleanup
		VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo;
		Utils::PopulateDebugMessageCreateInfo(debugMessengerCreateInfo, m_DebugMessenger->GetMessageCallback());
		instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugMessengerCreateInfo;
		#endif

		LF_CORE_ASSERT(vkCreateInstance(&instanceCreateInfo, nullptr, &Instance) == VK_SUCCESS, "Failed to create instance!");
	}

	void RenderContext::InitDevices() {

		// Pick physical device (= GPU to use)
		#ifdef LF_BE_OVERLYSPECIFIC
		Utils::ListVulkanPhysicalDevices(Instance);
		#endif

		PhysicalDevice = Utils::PickVulkanPhysicalDevice(Instance, Surface);

		// Create logical device
		// Figuring out what queues we want
		Utils::QueueFamilyIndices indices = Utils::IdentifyVulkanQueueFamilies(PhysicalDevice, Surface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.GraphicsFamilyIndex, indices.PresentFamilyIndex };
		float queuePriority = 1.0f;
		for (auto queueFamily : uniqueQueueFamilies) {

			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;

			queueCreateInfos.push_back(queueCreateInfo);
		}

		// Actually setting up the logical device info structs
		VkPhysicalDeviceFeatures logicalDeviceFeatures = {}; // No features needed for now
		logicalDeviceFeatures.samplerAnisotropy = VK_TRUE;

		VkDeviceCreateInfo logicalDeviceCreateInfo{};
		logicalDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		logicalDeviceCreateInfo.queueCreateInfoCount = (uint32_t)(queueCreateInfos.size());
		logicalDeviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		logicalDeviceCreateInfo.pEnabledFeatures = &logicalDeviceFeatures;
		logicalDeviceCreateInfo.enabledExtensionCount = (uint32_t)(Utils::requiredVulkanDeviceExtensions.size());
		logicalDeviceCreateInfo.ppEnabledExtensionNames = Utils::requiredVulkanDeviceExtensions.data();
		logicalDeviceCreateInfo.enabledLayerCount = (uint32_t)(DebugMessenger::ValidationLayers.size());
		logicalDeviceCreateInfo.ppEnabledLayerNames = DebugMessenger::ValidationLayers.data();
		logicalDeviceCreateInfo.pNext = nullptr;

		LF_CORE_ASSERT(vkCreateDevice(PhysicalDevice, &logicalDeviceCreateInfo, nullptr, &LogicalDevice) == VK_SUCCESS, "Failed to create logical device!");

		// Retrieve queue handles
		vkGetDeviceQueue(LogicalDevice, indices.GraphicsFamilyIndex, 0, &GraphicsQueueHandle);
		vkGetDeviceQueue(LogicalDevice, indices.PresentFamilyIndex, 0, &PresentQueueHandle);

		// Create Commandpool
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.queueFamilyIndex = indices.GraphicsFamilyIndex;

		LF_CORE_ASSERT(vkCreateCommandPool(LogicalDevice, &commandPoolCreateInfo, nullptr, &m_CommandPool) == VK_SUCCESS, "Failed to create command pool!");
	}

	VkCommandBuffer RenderContext::BeginSingleTimeCommandBuffer() {

		VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.commandPool = m_CommandPool;
		commandBufferAllocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(LogicalDevice, &commandBufferAllocInfo, &commandBuffer);

		VkCommandBufferBeginInfo commandBufferBeginInfo = {};
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

		return commandBuffer;
	}

	void RenderContext::EndSingleTimeCommandBuffer(VkCommandBuffer commandBuffer) {

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(GraphicsQueueHandle, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(GraphicsQueueHandle);

		vkFreeCommandBuffers(LogicalDevice, m_CommandPool, 1, &commandBuffer);
	}

	void RenderContext::CopyBuffer(Ref<Buffer> srcBuffer, Ref<Buffer> dstBuffer) {

		VkCommandBuffer commandBuffer = BeginSingleTimeCommandBuffer();

		VkBufferCopy copyRegion = {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = dstBuffer->GetSize();
		vkCmdCopyBuffer(commandBuffer, srcBuffer->GetBuffer(), dstBuffer->GetBuffer(), 1, &copyRegion);

		EndSingleTimeCommandBuffer(commandBuffer);
	}

	void RenderContext::CreateSyncObjects() {

		// Create sync objects
		ImageAvailableSemaphores.resize(m_MaxFramesInFlight);
		RenderFinishedSemaphores.resize(m_MaxFramesInFlight);
		InFlightFences.resize(m_MaxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < m_MaxFramesInFlight; i++) {

			LF_CORE_ASSERT(vkCreateSemaphore(LogicalDevice, &semaphoreCreateInfo, nullptr, &ImageAvailableSemaphores[i]) == VK_SUCCESS, "Failed to create imageAvailable semaphore!");
			LF_CORE_ASSERT(vkCreateSemaphore(LogicalDevice, &semaphoreCreateInfo, nullptr, &RenderFinishedSemaphores[i]) == VK_SUCCESS, "Failed to create renderFinished semaphore!");
			LF_CORE_ASSERT(vkCreateFence(LogicalDevice, &fenceCreateInfo, nullptr, &InFlightFences[i]) == VK_SUCCESS, "Failed to create inFlight fence!");
		}
	}

	void RenderContext::InitPipelines(VkDescriptorSetLayout descriptorSetLayout, VkVertexInputBindingDescription vertexBindingDescription, std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptions) {

		// Create Graphics pipeline
		VkAttachmentDescription colorAttachmentDescription = {};
		colorAttachmentDescription.format = m_SwapChain->GetImageFormat();
		colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentDescription depthAttachmentDescription = {};
		depthAttachmentDescription.format = m_SwapChain->GetDepthImage()->GetFormat();
		depthAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		GraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
		graphicsPipelineCreateInfo.VertexShaderPath = "Assets/Shaders/VertexShader.vert";
		graphicsPipelineCreateInfo.FragmentShaderPath = "Assets/Shaders/FragmentShader.frag";
		graphicsPipelineCreateInfo.Attachments = { colorAttachmentDescription, depthAttachmentDescription };
		graphicsPipelineCreateInfo.VertexAttributeDescriptions = vertexAttributeDescriptions;
		graphicsPipelineCreateInfo.VertexBindingDescription = vertexBindingDescription;
		graphicsPipelineCreateInfo.DescriptorSetLayout = descriptorSetLayout;

		m_GraphicsPipeline = Pipeline::CreateGraphicsPipeline(graphicsPipelineCreateInfo);

		// Important! The swapchain framebuffers need to be created after the pipeline is created.
		m_SwapChain->CreateFramebuffers();
	}
}