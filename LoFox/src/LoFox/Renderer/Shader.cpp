#include "lfpch.h"
#include "LoFox/Renderer/Shader.h"

#include <shaderc/shaderc.hpp>
// #include <spirv_cross/spirv_cross.hpp>
// #include <spirv_cross/spirv_glsl.hpp>

#include "LoFox/Utils/Utils.h"

namespace LoFox {

	static VkShaderStageFlagBits ShaderTypeToVulkan(ShaderType type) {

		switch (type) {
			case ShaderType::Vertex:	return VK_SHADER_STAGE_VERTEX_BIT;
			case ShaderType::Fragment:	return VK_SHADER_STAGE_FRAGMENT_BIT;
		}
		LF_CORE_ASSERT(false);
		return (VkShaderStageFlagBits)0;
	}

	static shaderc_shader_kind ShaderTypeToShaderC(ShaderType stage) {

		switch (stage) {

			case ShaderType::Vertex:	return shaderc_glsl_vertex_shader;
			case ShaderType::Fragment:	return shaderc_glsl_fragment_shader;
		}
		LF_CORE_ASSERT(false);
		return (shaderc_shader_kind)0;
	}

	void Shader::CreateShaderModule() {

		m_SourceCode = Utils::ReadFileAsString(m_Path);

		CompileVulkanBinaries();

		VkShaderModuleCreateInfo shaderModuleCreateInfo{};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = m_ByteCode.size() * sizeof(uint32_t);
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(m_ByteCode.data());
		
		LF_CORE_ASSERT(vkCreateShaderModule(m_Context->GetLogicalDevice(), &shaderModuleCreateInfo, nullptr, &m_ShaderModule) == VK_SUCCESS, "Failed to create shader module!");
	}

	Shader::Shader(Ref<RenderContext> context, const std::string& path, ShaderType type)
		: m_Context(context), m_Path(path), m_Type(type) {

		CreateShaderModule();

		m_CreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		m_CreateInfo.stage = ShaderTypeToVulkan(m_Type);
		m_CreateInfo.module = m_ShaderModule;
		m_CreateInfo.pName = "main";
	}

	Shader::~Shader() {

		vkDestroyShaderModule(m_Context->GetLogicalDevice(), m_ShaderModule, nullptr);
	}

	void Shader::CompileVulkanBinaries() {

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_0);
		const bool optimize = true;
		if (optimize)
			options.SetOptimizationLevel(shaderc_optimization_level_performance);

		shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(m_SourceCode, ShaderTypeToShaderC(m_Type), m_Path.c_str(), options);
		if (module.GetCompilationStatus() != shaderc_compilation_status_success) {

			LF_CORE_ERROR(module.GetErrorMessage());
			LF_CORE_ASSERT(false);
		}
		m_ByteCode = std::vector<uint32_t>(module.cbegin(), module.cend());
	}
}