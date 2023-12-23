#include "lfpch.h"
#include "LoFox/Renderer/Shader.h"

#ifdef LF_RENDERAPI_OPENGL
	#include "Platform/OpenGL/OpenGLShader.h"
#endif
#ifdef LF_RENDERAPI_VULKAN
	#include "Platform/Vulkan/VulkanShader.h"
#endif

namespace LoFox {

	Ref<Shader> Shader::Create(ShaderType type, const std::string& path) {
		
		#ifdef LF_RENDERAPI_OPENGL
			return CreateRef<OpenGLShader>(type, path);
		#endif
		#ifdef LF_RENDERAPI_VULKAN
			return CreateRef<VulkanShader>(type, path);
		#endif

		LF_CORE_ASSERT(false, "Unknown RendererAPI!");
	}

	Shader::Shader(ShaderType type, const std::string& path)
		: m_Path(path), m_Type(type) {}
}