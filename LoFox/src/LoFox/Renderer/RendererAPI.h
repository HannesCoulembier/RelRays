#pragma once
#include "LoFox/Core/Core.h"

namespace LoFox {

	class RendererAPI {

	public:
		enum class API {

			None = 0, Vulkan, OpenGL
		};
	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;

		inline static API GetAPI() { return s_API; }
		static Scope<RendererAPI> Create();
	private:
		static API s_API;
	};
}
