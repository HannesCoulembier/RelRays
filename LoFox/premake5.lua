project "LoFox"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "lfpch.h"
	pchsource "src/lfpch.cpp"

	files {

		"src/**.h",
		"src/**.cpp",
	}

	defines {

		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs {

		"src",
		"vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.VulkanSDK}",
	}

	links {

		"GLFW",
		"%{Library.Vulkan}",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "LF_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "LF_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "LF_DIST"
		runtime "Release"
		optimize "on"