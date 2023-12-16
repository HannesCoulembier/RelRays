#include "lfpch.h"
#include "Platform/OpenGL/OpenGLDebugMessenger.h"

namespace LoFox {

	void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam) {

		switch (severity) {

			case GL_DEBUG_SEVERITY_HIGH:		 LF_CORE_CRITICAL(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:		 LF_CORE_ERROR(message); return;
			case GL_DEBUG_SEVERITY_LOW:			 LF_CORE_WARN(message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: LF_CORE_TRACE(message); return;
		}

		LF_CORE_ASSERT(false, "Unknown severity level!");
	}


	Ref<OpenGLDebugMessenger> OpenGLDebugMessenger::Create() {
		return CreateRef<OpenGLDebugMessenger>();
	}

	OpenGLDebugMessenger::OpenGLDebugMessenger() {

		//#ifdef LF_DEBUG

			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLMessageCallback, nullptr);

			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE); // Enable all debuge messages
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE); // But disable notifications
			glDebugMessageControl(GL_DONT_CARE, GL_DEBUG_TYPE_OTHER, GL_DEBUG_SEVERITY_LOW, 0, NULL, GL_FALSE); // Also disable notifications of type "other" with a low severity (useless information, see what happens when turning it off)
		//#endif
	}
}