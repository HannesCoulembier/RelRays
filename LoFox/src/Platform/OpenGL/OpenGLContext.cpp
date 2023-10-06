#include "lfpch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace LoFox {

	void OpenGLContext::Init(GLFWwindow* windowHandle) {

		LF_CORE_ASSERT(windowHandle, "Window handle is null!");
		m_WindowHandle = windowHandle;

		glfwMakeContextCurrent(m_WindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LF_CORE_ASSERT(status, "Failed to initialize Glad");

		LF_CORE_INFO("OpenGL Info:");
		LF_CORE_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		LF_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		LF_CORE_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

		LF_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 3), "Benga requires at least OpenGL version 4.3");

	}

	void OpenGLContext::Shutdown() {

	}

	void OpenGLContext::BeginFrame(glm::vec3 clearColor) {

		glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLContext::EndFrame() {
		
	}

	void OpenGLContext::PresentFrame() {

		glfwSwapBuffers(m_WindowHandle);
	}

}
