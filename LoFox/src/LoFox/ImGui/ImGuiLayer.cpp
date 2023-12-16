#include "lfpch.h"
#include "LoFox/ImGui/ImGuiLayer.h"

#include <imgui.h>

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/ImGui/OpenGLImGuiLayer.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/ImGui/VulkanImGuiLayer.h"
#endif

#include "LoFox/Core/Application.h"

#include <GLFW/glfw3.h>

namespace LoFox {

	Ref<ImGuiLayer> ImGuiLayer::Create() {

		#ifdef LF_RENDERAPI_OPENGL
			return CreateRef<OpenGLImGuiLayer>();
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			return CreateRef<VulkanImGuiLayer>();
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	void ImGuiLayer::SetStyle() {

		ImGuiIO& io = ImGui::GetIO(); (void)io;

		// Add fonts
		float fontSize = 15.0f;
		io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", fontSize);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", fontSize);


		// Look at the style section in imgui_draw.cpp for reference
		auto& colors = ImGui::GetStyle().Colors;

		colors[ImGuiCol_WindowBg] =				ImVec4{ 0.13f,	0.13f,	0.13f,	1.0f };    
		colors[ImGuiCol_PopupBg] =				colors[ImGuiCol_WindowBg];				// Background of popups, menus, tooltips windows
		// colors[ImGuiCol_ChildBg] =			ImVec4{ 1.0f,	0.0f,	0.0f,	1.0f }; // Background of child windows

		// Tabs
		colors[ImGuiCol_Tab] =					ImVec4{ 0.9f,	0.45f,	0.25f,	1.0f };	// Unselected tabs
		colors[ImGuiCol_TabUnfocused] =			colors[ImGuiCol_Tab];					// Unselected tabs
		colors[ImGuiCol_TabUnfocusedActive] =	colors[ImGuiCol_Tab];					// Visible, unselected tab
		colors[ImGuiCol_TabActive] =			ImVec4{ 0.95f,	0.6f,	0.4f,	1.0f };	// Currently selected tab
		colors[ImGuiCol_TabHovered] =			ImVec4{ 0.9f,	0.7f,	0.5f,	1.0f };	// Hovered tab

		// Title
		colors[ImGuiCol_TitleBg] =				ImVec4{ 0.15f,	0.15f,	0.15f,	1.0f };
		colors[ImGuiCol_TitleBgActive] =		colors[ImGuiCol_TitleBg];
		colors[ImGuiCol_TitleBgCollapsed] =		ImVec4{ 0.5f,	0.3f,	0.2f,	1.0f };

		// Docking
		colors[ImGuiCol_DockingPreview] =		ImVec4{ 0.95f,	0.6f,	0.4f,	1.0f }; // Preview overlay color when about to docking something
		colors[ImGuiCol_DockingEmptyBg] =		colors[ImGuiCol_WindowBg];				// Background color for empty node (e.g. CentralNode with no window docked into it)

		// Separator
		colors[ImGuiCol_Separator] =			ImVec4{ 0.4f,	0.2f,	0.1f,	1.0f };
		// colors[ImGuiCol_SeparatorHovered] =	ImVec4{ 1.0f,	0.0f,	0.0f,	1.0f };
		// colors[ImGuiCol_SeparatorActive] =	ImVec4{ 1.0f,	0.0f,	0.0f,	1.0f };

		// Resize bars
		// colors[ImGuiCol_ResizeGrip] =		ImVec4{ 1.0f,	0.0f,	0.0f,	1.0f }; // Resize grip in lower-right and lower-left corners of windows.
		colors[ImGuiCol_ResizeGripHovered] =	ImVec4{ 0.4f,	0.2f,	0.1f,	1.0f };
		colors[ImGuiCol_ResizeGripActive] =		ImVec4{ 0.7f,	0.35f,	0.15f,	1.0f };

		// colors[ImGuiCol_Header] =			ImVec4{ 1.0f,	0.0f,	0.0f,	1.0f }; // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
		// colors[ImGuiCol_HeaderHovered] =		ImVec4{ 1.0f,	0.0f,	0.0f,	1.0f };
		// colors[ImGuiCol_HeaderActive] =		ImVec4{ 1.0f,	0.0f,	0.0f,	1.0f };
		
		// colors[ImGuiCol_Border] =			ImVec4{ 1.0f,	0.0f,	0.0f,	1.0f };
		// colors[ImGuiCol_BorderShadow] =		ImVec4{ 1.0f,	0.0f,	0.0f,	1.0f };

		// colors[ImGuiCol_FrameBg] =			ImVec4{ 1.0f,	0.0f,	0.0f,	1.0f }; // Background of checkbox, radio button, plot, slider, text input
		// colors[ImGuiCol_FrameBgHovered] =	ImVec4{ 1.0f,	0.0f,	0.0f,	1.0f };
		// colors[ImGuiCol_FrameBgActive] =		ImVec4{ 1.0f,	0.0f,	0.0f,	1.0f };


		// Remaining Color options taken from imgui.h:
		//	ImGuiCol_Text,
		//	ImGuiCol_TextDisabled,
		//	ImGuiCol_MenuBarBg,
		//	ImGuiCol_ScrollbarBg,
		//	ImGuiCol_ScrollbarGrab,
		//	ImGuiCol_ScrollbarGrabHovered,
		//	ImGuiCol_ScrollbarGrabActive,
		//	ImGuiCol_CheckMark,
		//	ImGuiCol_SliderGrab,
		//	ImGuiCol_SliderGrabActive,
		//	ImGuiCol_Button,
		//	ImGuiCol_ButtonHovered,
		//	ImGuiCol_ButtonActive,
		//	ImGuiCol_Header,                // Header* colors are used for CollapsingHeader, TreeNode, Selectable, MenuItem
		//	ImGuiCol_HeaderHovered,
		//	ImGuiCol_HeaderActive,
		//	ImGuiCol_PlotLines,
		//	ImGuiCol_PlotLinesHovered,
		//	ImGuiCol_PlotHistogram,
		//	ImGuiCol_PlotHistogramHovered,
		//	ImGuiCol_TableHeaderBg,         // Table header background
		//	ImGuiCol_TableBorderStrong,     // Table outer and header borders (prefer using Alpha=1.0 here)
		//	ImGuiCol_TableBorderLight,      // Table inner borders (prefer using Alpha=1.0 here)
		//	ImGuiCol_TableRowBg,            // Table row background (even rows)
		//	ImGuiCol_TableRowBgAlt,         // Table row background (odd rows)
		//	ImGuiCol_TextSelectedBg,
		//	ImGuiCol_DragDropTarget,        // Rectangle highlighting a drop target
		//	ImGuiCol_NavHighlight,          // Gamepad/keyboard: current highlighted item
		//	ImGuiCol_NavWindowingHighlight, // Highlight window when using CTRL+TAB
		//	ImGuiCol_NavWindowingDimBg,     // Darken/colorize entire screen behind the CTRL+TAB window list, when active
		//	ImGuiCol_ModalWindowDimBg,      // Darken/colorize entire screen behind a modal window, when one is active
	}

}
