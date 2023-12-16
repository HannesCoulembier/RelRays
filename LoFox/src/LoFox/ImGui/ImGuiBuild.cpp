#include "lfpch.h"

#ifdef LF_RENDERAPI_OPENGL
	#define IMGUI_IMPL_OPENGL_LOADER_GLAD
	#include <backends/imgui_impl_opengl3.cpp>
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include <backends/imgui_impl_vulkan.cpp>
#endif
#include <backends/imgui_impl_glfw.cpp>