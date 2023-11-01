#pragma once
#include "LoFox/Renderer/Pipeline.h"

#include <glad/glad.h>

#include "LoFox/Renderer/Resource.h"

namespace LoFox {

	GLenum TopologyToGLenum(Topology topology);

	struct OpenGLGraphicsPipelineData {

		GLuint ProgramID;
		GLenum PrimitiveTopology;
		Ref<ResourceLayout> ResourceLayout;
	};

	class OpenGLGraphicsPipeline : public GraphicsPipeline {

	public:
		OpenGLGraphicsPipeline(GraphicsPipelineCreateInfo createInfo);

		virtual void Destroy() override;
	private:
		GraphicsPipelineCreateInfo m_CreateInfo = {};
		OpenGLGraphicsPipelineData m_OpenGLData = {};
	};

	struct OpenGLComputePipelineData {

		GLuint ProgramID;
		Ref<ResourceLayout> ResourceLayout;
	};

	class OpenGLComputePipeline : public ComputePipeline {

	public:
		OpenGLComputePipeline(ComputePipelineCreateInfo createInfo);

		virtual void Dispatch(uint32_t width, uint32_t height, uint32_t groupWidth, uint32_t groupHeight);

		virtual void Destroy() override;
	private:
		ComputePipelineCreateInfo m_CreateInfo = {};
		OpenGLComputePipelineData m_OpenGLData = {};
	};
}