#pragma once

#include "LoFox/Renderer/Shader.h"
#include "LoFox/Renderer/Resource.h"
#include "LoFox/Renderer/VertexBuffer.h"

namespace LoFox {

	enum Topology {
		Triangle,
		LineStrip,
		Point,
	};

	struct GraphicsPipelineCreateInfo {

		Ref<Shader> VertexShader;
		Ref<Shader> FragmentShader;
		Ref<ResourceLayout> ResourceLayout = nullptr;
		VertexLayout VertexLayout;
		Topology Topology = Topology::Triangle;
		float LineWidth = 1.0f;
		float PointSize = 1.0f;
	};

	class GraphicsPipeline {

	public:
		virtual void Destroy() = 0;

		void* GetData() { return m_Data; }

		static Ref<GraphicsPipeline> Create(GraphicsPipelineCreateInfo createInfo);
	protected:
		void* m_Data = nullptr;
	};

	struct ComputePipelineCreateInfo {

		Ref<Shader> ComputeShader;
		Ref<ResourceLayout> ResourceLayout = nullptr;
	};

	class ComputePipeline {

	public:
		virtual void Destroy() = 0;

		virtual void Dispatch(uint32_t width, uint32_t height, uint32_t groupWidth, uint32_t groupHeight) = 0;

		void* GetData() { return m_Data; }

		static Ref<ComputePipeline> Create(ComputePipelineCreateInfo createInfo);
	protected:
		void* m_Data = nullptr;
	};
}