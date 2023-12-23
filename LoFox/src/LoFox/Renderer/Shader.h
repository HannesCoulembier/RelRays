#pragma once

namespace LoFox {

	// TODO: make ShaderType a bit field
	enum ShaderType {

		Vertex,
		Fragment,
		Compute,
	};

	class Shader {

	public:
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
	};
}