include "dependencies.lua"

workspace "RelRays"
	architecture "x86_64"
	startproject "Sandbox"

	configurations {
		"Debug-OpenGL",
		"Debug-Vulkan",
		"Release-OpenGL",
		"Release-Vulkan",
		"Dist-OpenGL",
		"Dist-Vulkan",
	}

	flags {
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "LoFox/vendor/GLFW"
	include "LoFox/vendor/Glad"
group ""

include "LoFox"
include "RelRays"
include "Sandbox"