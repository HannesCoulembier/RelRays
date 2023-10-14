#include "lfpch.h"
#include "Platform/OpenGL/OpenGLShader.h"

#include <shaderc/shaderc.hpp>

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

	static GLenum ShaderTypeToOpenGL(ShaderType stage) {

		switch (stage) {
			case ShaderType::Vertex:	return GL_VERTEX_SHADER;
			case ShaderType::Fragment:	return GL_FRAGMENT_SHADER;
			case ShaderType::Compute:	return GL_COMPUTE_SHADER;
		}
		LF_CORE_ASSERT(false);
		return (GLenum)0;
	}

	OpenGLShader::OpenGLShader(ShaderType type, const std::string& path)
		: Shader(type, path) {

		m_Data = &m_OpenGLData;

		LoadFile();
		CompileOpenGLBinaries();

		m_OpenGLData.ShaderID = glCreateShader(ShaderTypeToOpenGL(m_Type));
		const GLchar* sourceCStr = m_SourceCode.c_str();
		glShaderSource(m_OpenGLData.ShaderID, 1, &sourceCStr, 0);
		glCompileShader(m_OpenGLData.ShaderID);

		// TODO: add error checking using
		// glGetShaderiv(m_OpenGLData.ShaderID, GL_COMPILE_STATUS, &Result);
		// glGetShaderiv(m_OpenGLData.ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	}

	void OpenGLShader::LoadFile() {

		// TODO: implement caching system
		m_SourceCode = Utils::ReadFileAsString(m_Path);
	}

	void OpenGLShader::CompileOpenGLBinaries() {

		shaderc::Compiler compiler;
		shaderc::CompileOptions options;
		options.SetTargetEnvironment(shaderc_target_env_opengl, shaderc_env_version_opengl_4_5);
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

	void OpenGLShader::Destroy() {

	}
}