#include "lfpch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Platform/OpenGL/OpenGLPipeline.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Platform/OpenGL/OpenGLResource.h"

namespace LoFox {

	void OpenGLContext::Init(GLFWwindow* windowHandle) {

		LF_CORE_ASSERT(windowHandle, "Window handle is null!");
		m_WindowHandle = windowHandle;

		glfwMakeContextCurrent(m_WindowHandle);
		glfwSwapInterval(0); // Disables VSync
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LF_CORE_ASSERT(status, "Failed to initialize Glad");

		LF_CORE_INFO("OpenGL Info:");
		LF_CORE_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		LF_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		LF_CORE_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

		LF_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 3), "LoFox requires at least OpenGL version 4.3");

		glEnable(GL_FRAMEBUFFER_SRGB); // Transforms to correct colorspace when presenting framebuffer
		glEnable(GL_LINE_SMOOTH); // Makes LineWidth mathematically correct
		glEnable(GL_POINT_SMOOTH); // Makes points round

		glGenVertexArrays(1, &m_VertexArrayID);
		glBindVertexArray(m_VertexArrayID);
	}

	void OpenGLContext::Shutdown() {

		glDeleteVertexArrays(1, &m_VertexArrayID);
	}

	void OpenGLContext::BeginFrame(glm::vec3 clearColor) {

		glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLContext::SetActivePipeline(Ref<GraphicsPipeline> pipeline) {

		OpenGLGraphicsPipelineData* pipelineData = static_cast<OpenGLGraphicsPipelineData*>(pipeline->GetData());

		m_ActivePipeline = pipeline;
		glUseProgram(pipelineData->ProgramID);

		BindResourceLayout(pipelineData->ResourceLayout);
	}

	void OpenGLContext::Draw(Ref<VertexBuffer> vertexBuffer) {

		OpenGLVertexBufferData* vertexBufferData = static_cast<OpenGLVertexBufferData*>(vertexBuffer->GetData());
		OpenGLGraphicsPipelineData* pipelineData = static_cast<OpenGLGraphicsPipelineData*>(m_ActivePipeline->GetData());

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferData->Buffer);


		VertexLayout layout = vertexBufferData->VertexLayout;
		uint32_t attributeIndex = 0; // Increments for every attribute
		uint32_t offset = 0;
		for (VertexAttributeType attribute : layout.GetAttributes()) {

			switch (attribute) {

				case VertexAttributeType::Float:
				case VertexAttributeType::Float2:
				case VertexAttributeType::Float3:
				case VertexAttributeType::Float4: {

					glEnableVertexAttribArray(attributeIndex);
					glVertexAttribPointer(attributeIndex,
						GetVertexAttributeComponentCount(attribute),
						VertexAttributeToOpenGLBaseType(attribute),
						GL_FALSE, // TODO: add normalization to layout
						layout.VertexSize,
						(void*)offset);
					offset += GetVertexAttributeTypeSize(attribute);
					attributeIndex++;
					break;
				}
				case VertexAttributeType::Int:
				case VertexAttributeType::Int2:
				case VertexAttributeType::Int3:
				case VertexAttributeType::Int4:
				case VertexAttributeType::Bool: {

					glEnableVertexAttribArray(attributeIndex);
					glVertexAttribIPointer(attributeIndex,
						GetVertexAttributeComponentCount(attribute),
						VertexAttributeToOpenGLBaseType(attribute),
						layout.VertexSize,
						(void*)offset);
					offset += GetVertexAttributeTypeSize(attribute);
					attributeIndex++;
					break;
				}
				case VertexAttributeType::Mat3:
				case VertexAttributeType::Mat4: {

					uint8_t count = GetVertexAttributeComponentCount(attribute);
					for (uint8_t i = 0; i < count; i++) {

						glEnableVertexAttribArray(attributeIndex);
						glVertexAttribPointer(attributeIndex,
							count,
							VertexAttributeToOpenGLBaseType(attribute),
							GL_FALSE, // TODO: add normalizing option to layout (see benga)
							layout.VertexSize,
							(void*)(offset + sizeof(float) * count * i));
						glVertexAttribDivisor(attributeIndex, 1);
						attributeIndex++;
					}
					offset += GetVertexAttributeTypeSize(attribute);
					break;
				}
				default:
					LF_CORE_ASSERT(false, "Unknown VertexAttributeType!");
			}
		}

		glDrawArrays(pipelineData->PrimitiveTopology, 0, 3);
	}

	void OpenGLContext::EndFrame() {
		
	}

	void OpenGLContext::PresentFrame() {

		glfwSwapBuffers(m_WindowHandle);
	}

}
