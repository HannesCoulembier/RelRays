#pragma once

#include "LoFox/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace LoFox {

	class OpenGLContext : public GraphicsContext {

	public:
		static void Init(GLFWwindow* windowHandle);
		static void Shutdown();
		static void PresentFrame();
	private:
		inline static GLFWwindow* m_WindowHandle;
	};

}