#include "lfpch.h"
#include "Time.h"

#include "GLFW/glfw3.h"

namespace LoFox {

	float Time::GetTime() {
		return glfwGetTime();
	}
}