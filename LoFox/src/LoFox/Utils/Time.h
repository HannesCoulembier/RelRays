#pragma once

#include "LoFox/Core/Core.h"

#include <chrono>

namespace LoFox {

	class Time {

	public:
		static float GetTime();
	};

	class Timer {

	public:
		Timer() {
			Reset();
		}

		void Reset() {
			m_StartTime = std::chrono::high_resolution_clock::now();
		}

		float Elapsed() {
			return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_StartTime).count() * 0.001f * 0.001f * 0.001f;
		}

		float ElapsedMillis() {
			return Elapsed() * 1000.0f;
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTime;
	};

	class ScopedTimer {

	public:
		ScopedTimer(const std::string& name)
			: m_Name(name) {}
		
		~ScopedTimer() {

			  float time = m_Timer.ElapsedMillis();
			  LF_CORE_INFO("[TIMER] {0}: {1} ms", m_Name, time);
		}

	private:
		std::string m_Name;
		Timer m_Timer;
	};
}