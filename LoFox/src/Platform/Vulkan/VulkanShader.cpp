#include "lfpch.h"
#include "Platform/Vulkan/VulkanShader.h"

#include <shaderc/shaderc.hpp>

#include "Platform/Vulkan/VulkanContext.h"
#include "LoFox/Utils/Utils.h"

namespace LoFox {

	static shaderc_shader_kind ShaderTypeToShaderC(ShaderType stage) {
	
		switch (stage) {
			case ShaderType::Vertex:	return shaderc_glsl_vertex_shader;
			case ShaderType::Fragment:	return shaderc_glsl_fragment_shader;
			case ShaderType::Compute:	return shaderc_glsl_compute_shader;
		}
		LF_CORE_ASSERT(false);
		return (shaderc_shader_kind)0;
	}

	VulkanShader::VulkanShader(ShaderType type, const std::string& path)
		: Shader(type, path) {

		m_Data = &m_VulkanData;

		LoadFile();
		CompileVulkanBinaries();
		CreateShaderModule();
	}

	void VulkanShader::Destroy() {

		vkDestroyShaderModule(VulkanContext::LogicalDevice, m_VulkanData.ShaderModule, nullptr);
	}

	void VulkanShader::LoadFile() {

		// TODO: implement caching system
		m_SourceCode = Utils::ReadFileAsString(m_Path);
	}

	void VulkanShader::CompileVulkanBinaries() {
		
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

	void VulkanShader::CreateShaderModule() {

		VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = m_ByteCode.size() * sizeof(uint32_t);
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(m_ByteCode.data());

		LF_CORE_ASSERT(vkCreateShaderModule(VulkanContext::LogicalDevice, &shaderModuleCreateInfo, nullptr, &m_VulkanData.ShaderModule) == VK_SUCCESS, "Failed to create shader module!");
	}
}