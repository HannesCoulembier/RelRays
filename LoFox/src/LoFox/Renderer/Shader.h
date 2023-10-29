#pragma once

// #include "LoFox/Core/Core.h"
// 
// #include <vulkan/vulkan.h>

namespace LoFox {

	// TODO: make ShaderType a bit field
	enum ShaderType {

		Vertex,
		Fragment,
		Compute,
	};

	// VkShaderStageFlagBits ShaderTypeToVulkan(ShaderType type);

	class Shader {

	public:
		// Shader(const std::string& path, ShaderType type);
		// ~Shader();

		// inline VkPipelineShaderStageCreateInfo GetCreateInfo() { return m_CreateInfo; }

		virtual void Destroy() = 0;

		std::string GetPath() { return m_Path; }
		ShaderType GetShaderType() { return m_Type; }
		void* GetData() { return m_Data; }

		static Ref<Shader> Create(ShaderType type, const std::string& path);
	protected:
		Shader(ShaderType type, const std::string& path);
	protected:
		std::string m_Path = "";
		ShaderType m_Type;

		void* m_Data = nullptr;

		// std::string m_SourceCode;
		// std::vector<uint32_t> m_ByteCode;
		// 
		// VkShaderModule m_ShaderModule;
		// VkPipelineShaderStageCreateInfo m_CreateInfo = {};
	private:
		// void CreateShaderModule();

		// void CompileVulkanBinaries();
	};
}