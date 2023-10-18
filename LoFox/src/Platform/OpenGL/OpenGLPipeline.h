#pragma once

#include "LoFox/Renderer/Pipeline.h"

#include <glad/glad.h>

namespace LoFox {

	struct OpenGLGraphicsPipelineData {

		GLuint ProgramID;
		GLenum PrimitiveTopology;
	};

	class OpenGLGraphicsPipeline : public GraphicsPipeline {

	public:
		OpenGLGraphicsPipeline(GraphicsPipelineCreateInfo createInfo);

		virtual void Destroy() override;
	private:
		GraphicsPipelineCreateInfo m_CreateInfo = {};
		OpenGLGraphicsPipelineData m_OpenGLData = {};
	};
}