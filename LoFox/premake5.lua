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
		"vendor/stb_image/**.h",
		"vendor/stb_image/**.cpp",
		"vendor/glm/glm/**.inl",
		"vendor/glm/glm/**.hpp",
	}

	defines {

		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs {

		"src",
		"vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.stb_image}",
		"%{IncludeDir.VulkanSDK}",
	}

	links {

		"GLFW",
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug-OpenGL or configurations:Debug-Vulkan"
		defines "LF_DEBUG"
		runtime "Debug"
		symbols "on"
		
		links {

			"%{Library.ShaderC_Debug}",
			"%{Library.SPIRV_Cross_Debug}",
			"%{Library.SPIRV_Cross_GLSL_Debug}",
		}

	filter "configurations:Release-OpenGL or configurations:Release-Vulkan"
		defines "LF_RELEASE"
		runtime "Release"
		optimize "on"

		links {

			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}",
		}

	filter "configurations:Dist-OpenGL or configurations:Dist-Vulkan"
		defines "LF_DIST"
		runtime "Release"
		optimize "on"

		links {

			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}",
		}


	filter "configurations:Debug-OpenGL or configurations:Release-OpenGL or configurations:Dist-OpenGL"
		defines "LF_RENDERAPI_OPENGL"

		links {

			"Glad",
			"opengl32.lib",
		}

	filter "configurations:Debug-Vulkan or configurations:Release-Vulkan or configurations:Dist-Vulkan"
		defines "LF_RENDERAPI_VULKAN"
		
		links {

			"%{Library.Vulkan}",
		}