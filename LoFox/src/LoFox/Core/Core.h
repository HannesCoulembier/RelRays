#pragma once

#include <memory>

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

#include "Log.h"