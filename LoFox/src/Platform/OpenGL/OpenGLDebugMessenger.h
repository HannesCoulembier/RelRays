#pragma once

#include <glad/glad.h>

namespace LoFox {

	class OpenGLDebugMessenger {

	public:
		OpenGLDebugMessenger();

		static Ref<OpenGLDebugMessenger> Create();
	private:
	
	};
}