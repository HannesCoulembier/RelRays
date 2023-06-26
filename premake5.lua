include "dependencies.lua"

workspace "LoFox"
	architecture "x86_64"
	startproject "Sandbox"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	flags {
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "LoFox/vendor/GLFW"
group ""

include "LoFox"
include "Sandbox"