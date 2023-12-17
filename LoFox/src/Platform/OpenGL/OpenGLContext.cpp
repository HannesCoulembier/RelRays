#include "lfpch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Platform/OpenGL/OpenGLPipeline.h"
#include "Platform/OpenGL/OpenGLVertexBuffer.h"
#include "Platform/OpenGL/OpenGLIndexBuffer.h"
#include "Platform/OpenGL/OpenGLResource.h"

namespace LoFox {

	void OpenGLContext::Init(Ref<Window> window) {

		LF_CORE_ASSERT(window, "Window is null!");
		m_Window = window;

		glfwMakeContextCurrent(static_cast<GLFWwindow*>(m_Window->GetWindowHandle()));
		glfwSwapInterval(0); // Disables VSync
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		LF_CORE_ASSERT(status, "Failed to initialize Glad");

		LF_CORE_INFO("OpenGL Info:");
		LF_CORE_INFO("  Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		LF_CORE_INFO("  Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		LF_CORE_INFO("  Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

		LF_CORE_ASSERT(GLVersion.major > 4 || (GLVersion.major == 4 && GLVersion.minor >= 3), "LoFox requires at least OpenGL version 4.3");

		// Debug messenger
		m_DebugMessenger = OpenGLDebugMessenger::Create();

		// Settings
		// glEnable(GL_FRAMEBUFFER_SRGB); // Transforms to correct colorspace when presenting framebuffer
		glEnable(GL_LINE_SMOOTH); // Makes LineWidth mathematically correct
		glEnable(GL_POINT_SMOOTH); // Makes points round

		glEnable(GL_BLEND); // Enables blending
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Enables blending

		glEnable(GL_DEPTH_TEST);

		glGenVertexArrays(1, &m_VertexArrayID);
		glBindVertexArray(m_VertexArrayID);


		// Framebuffer
		// glGenFramebuffers(1, &m_FramebufferID);
		// glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferID);
		// 
		// 
		// glGenTextures(1, &m_FramebufferColorAttachmentID);
		// glBindTexture(GL_TEXTURE_2D, m_FramebufferColorAttachmentID);
		// // Attach color texture
		// int width, height;
		// glfwGetFramebufferSize(static_cast<GLFWwindow*>(m_Window->GetWindowHandle()), &width, &height);
		// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		// 
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FramebufferColorAttachmentID, 0);
		// 
		// glGenRenderbuffers(1, &m_RenderbufferID);
		// glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID);
		// glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
		// glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID);
		// 
		// LF_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete!");
		// 
		// glBindFramebuffer(GL_FRAMEBUFFER, 0);
		// glBindTexture(GL_TEXTURE_2D, 0);
		// glBindRenderbuffer(GL_RENDERBUFFER, 0);
	}

	void OpenGLContext::Shutdown() {

		glDeleteVertexArrays(1, &m_VertexArrayID);
	}

	void OpenGLContext::BeginFrame() {

	}

	void OpenGLContext::BeginFramebuffer(Ref<Framebuffer> framebuffer, glm::vec3 clearColor) {

		m_ActiveFramebuffer = framebuffer;
		OpenGLFramebufferData* framebufferData = static_cast<OpenGLFramebufferData*>(m_ActiveFramebuffer->GetData());
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferData->RendererID);
		glViewport(0, 0, m_ActiveFramebuffer->GetWidth(), m_ActiveFramebuffer->GetHeight());
		glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLContext::SetActivePipeline(Ref<GraphicsPipeline> pipeline) {

		OpenGLFramebufferData* framebufferData = static_cast<OpenGLFramebufferData*>(m_ActiveFramebuffer->GetData());
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferData->RendererID);

		OpenGLGraphicsPipelineData* pipelineData = static_cast<OpenGLGraphicsPipelineData*>(pipeline->GetData());

		m_ActivePipeline = pipeline;
		glUseProgram(pipelineData->ProgramID);

		BindResourceLayout(pipelineData->ProgramID, pipelineData->ResourceLayout);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLContext::Draw(Ref<IndexBuffer> indexBuffer, Ref<VertexBuffer> vertexBuffer) {

		OpenGLFramebufferData* framebufferData = static_cast<OpenGLFramebufferData*>(m_ActiveFramebuffer->GetData());
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferData->RendererID);

		OpenGLVertexBufferData* vertexBufferData = static_cast<OpenGLVertexBufferData*>(vertexBuffer->GetData());
		OpenGLIndexBufferData* indexBufferData = static_cast<OpenGLIndexBufferData*>(indexBuffer->GetData());
		OpenGLGraphicsPipelineData* pipelineData = static_cast<OpenGLGraphicsPipelineData*>(m_ActivePipeline->GetData());

		glBindBuffer(GL_ARRAY_BUFFER, vertexBufferData->Buffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferData->Buffer);


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

		glDrawElements(pipelineData->PrimitiveTopology, indexBuffer->GetNumberOfIndices(), GL_UNSIGNED_INT, nullptr);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void OpenGLContext::EndFramebuffer() {
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		m_ActiveFramebuffer = nullptr;
	}

	void OpenGLContext::EndFrame() {

	}

	void OpenGLContext::PresentFrame() {

		glfwSwapBuffers(static_cast<GLFWwindow*>(m_Window->GetWindowHandle()));
	}

}
