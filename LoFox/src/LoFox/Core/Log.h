#pragma once

#include "Core.h"

#include <spdlog/spdlog.h>

namespace LoFox {

	class Log {

	public:
		static void Init();

		inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

// Engine logging
#define LF_CORE_TRACE(...)		::LoFox::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define LF_CORE_INFO(...)		::LoFox::Log::GetCoreLogger()->info(__VA_ARGS__)
#define LF_CORE_WARN(...)		::LoFox::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define LF_CORE_ERROR(...)		::LoFox::Log::GetCoreLogger()->error(__VA_ARGS__)
#define LF_CORE_CRITICAL(...)	::LoFox::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client logging
#define LF_TRACE(...)		::LoFox::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LF_INFO(...)		::LoFox::Log::GetClientLogger()->info(__VA_ARGS__)
#define LF_WARN(...)		::LoFox::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LF_ERROR(...)		::LoFox::Log::GetClientLogger()->error(__VA_ARGS__)
#define LF_CRITICAL(...)	::LoFox::Log::GetClientLogger()->critical(__VA_ARGS__)