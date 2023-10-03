#pragma once

#include "LoFox/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace LoFox {

	class OpenGLContext : public GraphicsContext {

	public:
		static void Init(GLFWwindow* windowHandle);
		static void Shutdown();

		static void BeginFrame(glm::vec3 clearColor);
		static void EndFrame();

		static void PresentFrame();
	private:
		inline static GLFWwindow* m_WindowHandle;
	};

}