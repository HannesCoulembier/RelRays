#pragma once

#include "LoFox/Core/Core.h"

#include <vulkan/vulkan.h>

namespace LoFox {

	enum ShaderType {

		Vertex,
		Fragment,
		Compute,
	};

	class Shader {

	public:
		Shader(const std::string& path, ShaderType type);
		~Shader();

		inline VkPipelineShaderStageCreateInfo GetCreateInfo() { return m_CreateInfo; }
	private:
		void CreateShaderModule();

		void CompileVulkanBinaries();
	private:
		const std::string& m_Path;
		ShaderType m_Type;

		std::string m_SourceCode;
		std::vector<uint32_t> m_ByteCode;

		VkShaderModule m_ShaderModule;
		VkPipelineShaderStageCreateInfo m_CreateInfo = {};
	};
}