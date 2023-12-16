#include "lfpch.h"
#include "Platform/Vulkan/ImGui/VulkanImGuiLayer.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanFramebuffer.h"
#include "Platform/Vulkan/Utils.h"

#include "LoFox/Core/Application.h"

#include <GLFW/glfw3.h>

namespace LoFox {

	void VulkanImGuiLayer::OnAttach() {

		// Create Vulkan Structures
		VkAttachmentDescription colorAttachment = {}; // The image we will render into
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // After the renderpass ends, the image will be displayed to the screen.
		colorAttachment.format = VulkanContext::Swapchain->GetImageFormat(); // Use the format of the swapchain
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // Don't clear the attachment
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // We don't use MSAA
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE; // We don't care about stencil
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Keep the attachment stored when the renderpass ends

		std::vector<VkAttachmentDescription> attachments = { colorAttachment };

		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0; // Attachment is at index 0 in attachments array
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		std::vector<VkAttachmentReference> attachmentRefs = { colorAttachmentRef };

		VkSubpassDescription subpass = {};
		subpass.colorAttachmentCount = static_cast<uint32_t>(attachmentRefs.size());
		subpass.pColorAttachments = attachmentRefs.data();
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

		VkSubpassDependency dependency = {}; // Synchronising with the other render pass
		// dependency.dependencyFlags = 
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // Pixels are already written to the framebuffer
		dependency.dstSubpass = 0; // The only and first subpass in m_ImGuiRenderPass
		dependency.srcAccessMask = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // ImGui draws to the same target
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // Reference last subpass of the previous renderpass

		std::vector<VkSubpassDescription> subpasses = { subpass };
		std::vector<VkSubpassDependency> dependencies = { dependency };

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.pSubpasses = subpasses.data();
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.subpassCount = subpasses.size();
		renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
		renderPassInfo.pDependencies = dependencies.data();

		LF_CORE_ASSERT(vkCreateRenderPass(VulkanContext::LogicalDevice, &renderPassInfo, nullptr, &m_ImGuiRenderPass) == VK_SUCCESS, "Failed to create renderpass!");

		// DescriptorPool
		std::vector<VkDescriptorPoolSize> sizes = {
			{ VK_DESCRIPTOR_TYPE_SAMPLER,					1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,				1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,				1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,		1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,		1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,			1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,	1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,	1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,			1000 }
		};

		VkDescriptorPoolCreateInfo poolCreateInfo = {};
		poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolCreateInfo.maxSets = 1000;
		poolCreateInfo.pNext = nullptr;
		poolCreateInfo.poolSizeCount = (uint32_t)sizes.size();
		poolCreateInfo.pPoolSizes = sizes.data();
		poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;

		LF_CORE_ASSERT(vkCreateDescriptorPool(VulkanContext::LogicalDevice, &poolCreateInfo, nullptr, &m_ImGuiDescriptorPool) == VK_SUCCESS, "Failed to create ImGui descriptor pool!");

		// CommandPool
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		commandPoolCreateInfo.pNext = nullptr;
		commandPoolCreateInfo.queueFamilyIndex = Utils::IdentifyVulkanQueueFamilies(VulkanContext::PhysicalDevice, VulkanContext::Surface).GraphicsFamilyIndex;
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

		LF_CORE_ASSERT(vkCreateCommandPool(VulkanContext::LogicalDevice, &commandPoolCreateInfo, nullptr, &m_ImGuiCommandPool) == VK_SUCCESS, "Failed to create command pool!");
		// CommandBuffers
		m_ImGuiCommandBuffers.resize(VulkanContext::Swapchain->GetImageCount());
		VkCommandBufferAllocateInfo commandBufferAllocInfo = {};
		commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocInfo.commandBufferCount = m_ImGuiCommandBuffers.size();
		commandBufferAllocInfo.commandPool = m_ImGuiCommandPool;
		commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocInfo.pNext = nullptr;
		LF_CORE_ASSERT(vkAllocateCommandBuffers(VulkanContext::LogicalDevice, &commandBufferAllocInfo, &m_ImGuiCommandBuffers[0]) == VK_SUCCESS, "Failed to allocate main command buffer!");


		// Setup Dear ImGui context ---------------------------------------------
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}


		SetStyle();


		// Init ImGui for Vulkan
		Application& app = Application::GetInstance();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetActiveWindow()->GetWindowHandle());

		ImGui_ImplGlfw_InitForVulkan(window, true);

		uint32_t swapchainImageCount = VulkanContext::Swapchain->GetImageCount();

		ImGui_ImplVulkan_InitInfo initInfo = {};
		initInfo.Instance = VulkanContext::Instance;
		initInfo.PhysicalDevice = VulkanContext::PhysicalDevice;
		initInfo.Device = VulkanContext::LogicalDevice;
		initInfo.QueueFamily = Utils::IdentifyVulkanQueueFamilies(VulkanContext::PhysicalDevice, VulkanContext::Surface).GraphicsFamilyIndex;
		initInfo.Queue = VulkanContext::GraphicsQueueHandle;
		initInfo.PipelineCache = VK_NULL_HANDLE;
		initInfo.DescriptorPool = m_ImGuiDescriptorPool;
		initInfo.Subpass = 0;
		initInfo.MinImageCount = VulkanContext::Swapchain->GetMinImageCount();
		initInfo.ImageCount = swapchainImageCount;
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		initInfo.Allocator = nullptr;
		initInfo.CheckVkResultFn = nullptr;
		ImGui_ImplVulkan_Init(&initInfo, m_ImGuiRenderPass);
		
		
		// upload fonts to GPU
		ImGui_ImplVulkan_CreateFontsTexture();


		// Framebuffers
		m_ImGuiFramebuffers = std::vector<Ref<Framebuffer>>(swapchainImageCount);
		for (int i = 0; i < swapchainImageCount; i++) {

			m_ImGuiFramebuffers[i] = VulkanFramebuffer::CreateFromImageView(VulkanContext::Swapchain->GetExtent().width, VulkanContext::Swapchain->GetExtent().height, VulkanContext::Swapchain->GetImageView(i), m_ImGuiRenderPass);
		}

		VulkanContext::Swapchain->SubscribeToDeath(std::bind(&VulkanImGuiLayer::RecreateFramebuffers, this));
	}

	void VulkanImGuiLayer::OnDetach() {

		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		vkDestroyDescriptorPool(VulkanContext::LogicalDevice, m_ImGuiDescriptorPool, nullptr);
		vkDestroyRenderPass(VulkanContext::LogicalDevice, m_ImGuiRenderPass, nullptr);
		vkDestroyCommandPool(VulkanContext::LogicalDevice, m_ImGuiCommandPool, nullptr);
		for (auto fb : m_ImGuiFramebuffers)
			fb->Destroy();
	}

	void VulkanImGuiLayer::OnEvent(Event& e) {

		if (m_BlockEvents) {

			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void VulkanImGuiLayer::Begin() {

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void VulkanImGuiLayer::End() {

		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::GetInstance();
		io.DisplaySize = ImVec2((float)app.GetActiveWindow()->GetWindowData().Width, (float)app.GetActiveWindow()->GetWindowData().Height);

		// Rendering
		ImGui::Render();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			// GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			// glfwMakeContextCurrent(backup_current_context);
		}

		VkCommandBuffer commandBuffer = m_ImGuiCommandBuffers[VulkanContext::Swapchain->GetThisFramesImageIndex()];
		Ref<Framebuffer> framebuffer = m_ImGuiFramebuffers[VulkanContext::Swapchain->GetThisFramesImageIndex()];
		
		VulkanContext::BeginFramebuffer(framebuffer, commandBuffer, m_ImGuiRenderPass, { 1.0f, 0.0f, 1.0f });
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
		VulkanContext::EndFramebuffer();
	}

	void VulkanImGuiLayer::RecreateFramebuffers() {

		// Can't call framebuffer->Recreate() because number of framebuffers might have changed
		for (auto fb : m_ImGuiFramebuffers)
			fb->Destroy();

		uint32_t swapchainImageCount = VulkanContext::Swapchain->GetImageCount();
		m_ImGuiFramebuffers = std::vector<Ref<Framebuffer>>(swapchainImageCount);
		for (int i = 0; i < swapchainImageCount; i++) {

			m_ImGuiFramebuffers[i] = VulkanFramebuffer::CreateFromImageView(VulkanContext::Swapchain->GetExtent().width, VulkanContext::Swapchain->GetExtent().height, VulkanContext::Swapchain->GetImageView(i), m_ImGuiRenderPass);
		}
	}
}
