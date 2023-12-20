#include "lfpch.h"
#include "Platform/OpenGL/OpenGLPipeline.h"

#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLResource.h"

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

		m_OpenGLData.ResourceLayout = m_CreateInfo.ResourceLayout;

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

		// Check for errors
		GLint isLinked;
		glGetProgramiv(m_OpenGLData.ProgramID, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE) {

			GLint maxLength;
			glGetProgramiv(m_OpenGLData.ProgramID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_OpenGLData.ProgramID, maxLength, &maxLength, infoLog.data());
			LF_CORE_ERROR("Shader linking failed:\n{0}", infoLog.data());
		}
	}

	void OpenGLGraphicsPipeline::Destroy() {

	}

	OpenGLComputePipeline::OpenGLComputePipeline(ComputePipelineCreateInfo createInfo)
		: m_CreateInfo(createInfo) {

		m_Data = &m_OpenGLData;

		m_OpenGLData.ResourceLayout = m_CreateInfo.ResourceLayout;

		// Program
		m_OpenGLData.ProgramID = glCreateProgram();

		GLuint computeShaderID = static_cast<OpenGLShaderData*>(m_CreateInfo.ComputeShader->GetData())->ShaderID;
		glAttachShader(m_OpenGLData.ProgramID, computeShaderID);
		glLinkProgram(m_OpenGLData.ProgramID);

		// Check for errors
		GLint isLinked;
		glGetProgramiv(m_OpenGLData.ProgramID, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE) {

			GLint maxLength;
			glGetProgramiv(m_OpenGLData.ProgramID, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(m_OpenGLData.ProgramID, maxLength, &maxLength, infoLog.data());
			LF_CORE_ERROR("Shader linking failed ({0}):\n{1}", m_CreateInfo.ComputeShader->GetPath(), infoLog.data());
		}
	}

	void OpenGLComputePipeline::Dispatch(uint32_t width, uint32_t height, uint32_t groupWidth, uint32_t groupHeight) {

		glUseProgram(m_OpenGLData.ProgramID);
		BindResourceLayout(m_OpenGLData.ProgramID, m_OpenGLData.ResourceLayout);
		glDispatchCompute((uint32_t)((float)width / (float)groupWidth), (uint32_t)((float)height / (float)groupHeight), 1);
	}

	void OpenGLComputePipeline::Destroy() {

	}
}