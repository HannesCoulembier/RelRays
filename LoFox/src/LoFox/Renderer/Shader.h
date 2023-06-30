#pragma once

#include "LoFox/Core/Core.h"

#include "LoFox/Renderer/RenderContext.h"

namespace LoFox {

	enum ShaderType {

		Vertex,
		Fragment,
	};

	class Shader {

	public:
		Shader(Ref<RenderContext> context, const std::string& path, ShaderType type);
		~Shader();

		VkPipelineShaderStageCreateInfo GetCreateInfo() { return m_CreateInfo; }
	private:
		void CreateShaderModule();

		void CompileVulkanBinaries();
	private:
		Ref<RenderContext> m_Context;

		const std::string& m_Path;
		ShaderType m_Type;

		std::string m_SourceCode;
		std::vector<uint32_t> m_ByteCode;

		VkShaderModule m_ShaderModule;
		VkPipelineShaderStageCreateInfo m_CreateInfo = {};
	};
}