#include "lfpch.h"
#include "LoFox/Renderer/Renderer.h"

namespace LoFox {

	Ref<Window> Renderer::m_Window;
	Ref<RenderContext> Renderer::m_Context;

	void Renderer::Init(Ref<Window> window) {

		m_Window = window;

		m_Context = RenderContext::Create();
		m_Context->Init(m_Window);
	}

	void Renderer::Shutdown() {

		m_Context->Shutdown();
	}
}