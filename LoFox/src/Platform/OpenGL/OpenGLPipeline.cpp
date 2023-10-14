#include "lfpch.h"
#include "Platform/OpenGL/OpenGLPipeline.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace LoFox {

	OpenGLGraphicsPipeline::OpenGLGraphicsPipeline(GraphicsPipelineCreateInfo createInfo)
		: m_CreateInfo(createInfo) {

		m_Data = &m_OpenGLData;

		// Program
		m_OpenGLData.ProgramID = glCreateProgram();

		GLuint vertexShaderID = static_cast<OpenGLShaderData*>(m_CreateInfo.VertexShader->GetData())->ShaderID;
		GLuint fragmentShaderID = static_cast<OpenGLShaderData*>(m_CreateInfo.FragmentShader->GetData())->ShaderID;
		glAttachShader(m_OpenGLData.ProgramID, vertexShaderID);
		glAttachShader(m_OpenGLData.ProgramID, fragmentShaderID);
		glLinkProgram(m_OpenGLData.ProgramID);

		// TODO: add error checking using:
		// glGetProgramiv(m_OpenGLData.ProgramID, GL_LINK_STATUS, &Result);
		// glGetProgramiv(m_OpenGLData.ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	}

	void OpenGLGraphicsPipeline::Destroy() {

	}
}