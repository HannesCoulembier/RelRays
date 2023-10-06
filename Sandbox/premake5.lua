project "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files {

		"src/**.h",
		"src/**.cpp"
	}

	includedirs {

		"%{wks.location}/LoFox/vendor/spdlog/include",
		"%{wks.location}/LoFox/src",
		"%{wks.location}/LoFox/vendor",
		"%{IncludeDir.glm}",
		-- "%{IncludeDir.VulkanSDK}",
	}

	links {

		"LoFox"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug-OpenGL or configurations:Debug-Vulkan"
		defines "LF_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release-OpenGL or configurations:Release-Vulkan"
		defines "LF_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist-OpenGL or configurations:Dist-Vulkan"
		defines "LF_DIST"
		runtime "Release"
		optimize "on"
		kind "WindowedApp"
		entrypoint "mainCRTStartup"