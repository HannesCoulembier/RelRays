#pragma once
#include "LoFox/Renderer/Shader.h"

#include <glad/glad.h>

namespace LoFox {

	struct OpenGLShaderData {

		GLuint ShaderID;
	};

	class OpenGLShader : public Shader {

	public:
		OpenGLShader(ShaderType type, const std::string& path);

		virtual void Destroy() override;
	private:
		void LoadFile();
		void CompileOpenGLBinaries();
	private:
		OpenGLShaderData m_OpenGLData = {};

		std::string m_SourceCode = "";
		std::vector<uint32_t> m_ByteCode = {};
	};
}