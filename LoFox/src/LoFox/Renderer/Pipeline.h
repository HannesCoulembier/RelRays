#pragma once

#include "LoFox/Renderer/Shader.h"
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
}




// #include "LoFox/Core/Core.h"
// 
// #include <vulkan/vulkan.h>
// 
// #include "LoFox/Renderer/VertexBuffer.h"
// #include "LoFox/Renderer/Resource.h"
// 
// namespace LoFox {
// 
// 	struct ComputePipelineCreateInfo {
// 
// 		std::string ComputeShaderPath = "";
// 
// 		Ref<ResourceLayout> ResourceLayout;
// 	};
// 
// 	class ComputePipeline {
// 
// 	public:
// 		ComputePipelineCreateInfo CreateInfo;
// 		VkPipeline Pipeline;
// 		VkPipelineLayout Layout;
// 
// 		void PushConstant(uint32_t index, const void* data);
// 
// 		void Destroy();
// 
// 		void Bind();
// 		void Dispatch(uint32_t width, uint32_t height, uint32_t groupWidth, uint32_t groupHeight);
// 
// 		std::vector<VkPushConstantRange> GetPushConstants() { return m_PushConstants; }
// 		std::vector<const void*> GetPushConstantsData() { return m_PushConstantsData; }
// 	private:
// 		void InitLayout();
// 		void InitDescriptorSets();
// 		void InitPipeline();
// 
// 		void CreateDescriptorPool();
// 	private:
// 		uint32_t m_PushConstantsTotalOffset;
// 		VkShaderStageFlags m_PushConstantsStagesUsed;
// 		std::vector<VkPushConstantRange> m_PushConstants;
// 		std::vector<const void*> m_PushConstantsData;
// 
// 		VkDescriptorPool m_DescriptorPool;
// 		std::vector<VkDescriptorSet> m_DescriptorSets;
// 
// 		friend class ComputePipelineBuilder;
// 	};
// 
// 	class ComputePipelineBuilder {
// 
// 	public:
// 		ComputePipelineBuilder(ComputePipelineCreateInfo createInfo);
// 		Ref<ComputePipeline> CreateComputePipeline();
// 
// 		void PreparePushConstant(uint32_t objectSize, ShaderType shaderStage);
// 	private:
// 		ComputePipelineCreateInfo m_CreateInfo;
// 		Ref<ComputePipeline> m_Pipeline;
// 	};
// 
// 	struct GraphicsPipelineCreateInfo {
// 
// 		std::string VertexShaderPath = "";
// 		std::string FragmentShaderPath = "";
// 
// 		Ref<ResourceLayout> ResourceLayout;
// 	};
// 
// 	class GraphicsPipeline {
// 
// 	public:
// 		GraphicsPipelineCreateInfo CreateInfo;
// 		VkPipeline Pipeline;
// 		VkRenderPass RenderPass;
// 		VkPipelineLayout Layout;
// 
// 		void PushConstant(uint32_t index, const void* data);
// 
// 		void Destroy();
// 
// 		std::vector<VkPushConstantRange> GetPushConstants() { return m_PushConstants; }
// 		std::vector<const void*> GetPushConstantsData() { return m_PushConstantsData; }
// 	private:
// 		void InitLayout();
// 		void InitRenderPass();
// 		void InitPipeline();
// 	private:
// 		uint32_t m_PushConstantsTotalOffset;
// 		VkShaderStageFlags m_PushConstantsStagesUsed;
// 		std::vector<VkPushConstantRange> m_PushConstants;
// 		std::vector<const void*> m_PushConstantsData;
// 
// 		Ref<VertexBuffer> m_VertexBuffer;
// 
// 		friend class GraphicsPipelineBuilder;
// 	};
// 
// 	class GraphicsPipelineBuilder {
// 
// 	public:
// 		GraphicsPipelineBuilder(GraphicsPipelineCreateInfo createInfo);
// 		Ref<GraphicsPipeline> CreateGraphicsPipeline();
// 
// 		void PreparePushConstant(uint32_t objectSize, ShaderType shaderStage);
// 		void PrepareVertexBuffer(Ref<VertexBuffer> buffer);
// 	private:
// 		GraphicsPipelineCreateInfo m_CreateInfo;
// 		Ref<GraphicsPipeline> m_Pipeline;
// 	};
// }