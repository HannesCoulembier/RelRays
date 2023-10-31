#pragma once
#include "LoFox/Renderer/Shader.h"

#include <vulkan/vulkan.h>

namespace LoFox {

	VkShaderStageFlagBits ShaderTypeToVulkanShaderStage(ShaderType type);

	struct VulkanShaderData {

		VkShaderModule ShaderModule;
	};

	class VulkanShader : public Shader {

	public:
		VulkanShader(ShaderType type, const std::string& path);

		virtual void Destroy() override;
	private:
		void LoadFile();
		void CompileVulkanBinaries();
		void CreateShaderModule();
	private:
		VulkanShaderData m_VulkanData = {};

		std::string m_SourceCode = "";
		std::vector<uint32_t> m_ByteCode = {};
	};
}