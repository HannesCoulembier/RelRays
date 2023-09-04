#pragma once

#include <memory>

// Will give A LOT of debug info (that you propably don't need)
/*
Enables progress messages like: "Creating application named: name_of_app"
Shows all Vulkan messages (otherwise only messages with severity warning or higer are shown)
Lists as much information as possible (all available Vulkan extensions, layers, application name, window size, ...)
*/
#ifdef LF_DEBUG
	#define LF_BE_OVERLYSPECIFIC

	#define LF_USE_VULKAN_VALIDATION_LAYERS
#endif

// Platform detection
#ifdef _WIN32
	// Windows x86/x64
	#define LF_PLATFORM_WINDOWS

	#ifdef _WIN64
	#else
		#error "x86 Builds are not supported!"
	#endif
#else
	#error "LoFox only supports Windows"
#endif

// Configures debug break
#ifdef LF_DEBUG
	#if defined(LF_PLATFORM_WINDOWS)
		#define LF_DEBUGBREAK() __debugbreak()
	#elif defined(LF_PLATFORM_LINUX)
		#include <signal.h>
		#define LF_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif
	#define LF_ENABLE_ASSERTS()
#else
	#define LF_DEBUGBREAK()
#endif

// Helper macros
#define LF_EXPAND_MACRO(x) x
#define LF_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define LF_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

// Define Scope and Ref (unique and shared pointer decorations)
namespace LoFox {

	template<typename T>
	using Scope = std::unique_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args) {
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;
	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#include "LoFox/Core/Log.h"
#include "LoFox/Core/Assert.h"