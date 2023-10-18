#include "lfpch.h"
#include "Platform/OpenGL/OpenGLPipeline.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace LoFox {

	GLenum TopologyToGLenum(Topology topology) {

		switch (topology) {
			case Topology::Triangle: return GL_TRIANGLES;
			case Topology::LineStrip: return GL_LINE_STRIP;
			case Topology::Point: return GL_POINTS;
		}

		LF_CORE_ASSERT("Unknown Topology!");
		return GLenum(0);
	}

	OpenGLGraphicsPipeline::OpenGLGraphicsPipeline(GraphicsPipelineCreateInfo createInfo)
		: m_CreateInfo(createInfo) {

		m_Data = &m_OpenGLData;

		m_OpenGLData.PrimitiveTopology = TopologyToGLenum(m_CreateInfo.Topology);
		glLineWidth(m_CreateInfo.LineWidth);
		glPointSize(m_CreateInfo.PointSize);

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