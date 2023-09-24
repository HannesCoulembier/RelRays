#pragma once

#include "LoFox/Core/Core.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

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

// Will give A LOT of debug info (that you propably don't need)
#ifdef LF_BE_OVERLYSPECIFIC
#define LF_OVERSPECIFY(...) LF_CORE_INFO(__VA_ARGS__)
#else
#define LF_OVERSPECIFY(...)
#endif

template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& matrix) {

	return os << glm::to_string(matrix);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::qua<T, Q>& quaternion) {

	return os << glm::to_string(quaternion);
}