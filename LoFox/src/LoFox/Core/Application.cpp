#include "lfpch.h"
#include "LoFox/Core/Application.h"

#include "LoFox/Core/Log.h"

#include <vulkan/vulkan.h>

#include "LoFox/Utils/VulkanUtils.h"
#include "LoFox/Utils/Utils.h"
#include "LoFox/Utils/Time.h"

namespace LoFox {

	Application::Application(const ApplicationSpec& spec)
		: m_Spec(spec) {

		LF_OVERSPECIFY("Creating application named \"{0}\":\n", m_Spec.Name);

		m_Window = Window::Create({ m_Spec.Name, 1720, 960 });

		m_RenderContext = RenderContext::Create();
		m_RenderContext->Init(m_Window);

		LF_OVERSPECIFY("Creation of application \"{0}\" complete.\n", m_Spec.Name);
	}

	void Application::Run() {

		// Logger test
		/*
		LF_CORE_TRACE("TEST");
		LF_CORE_INFO("TEST");
		LF_CORE_WARN("TEST");
		LF_CORE_ERROR("TEST");
		LF_CORE_CRITICAL("TEST");

		LF_TRACE("TEST");
		LF_INFO("TEST");
		LF_WARN("TEST");
		LF_ERROR("TEST");
		LF_CRITICAL("TEST");
		*/

		while (!m_Window->ShouldClose()) {

			float time = Time::GetTime();
			float timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			m_Window->OnUpdate();

			for (auto& layer : m_LayerStack)
				layer->OnUpdate(timestep);

			m_RenderContext->OnRender();
		}

		m_RenderContext->WaitIdle();
	}

	Application::~Application() {

		for (auto& layer : m_LayerStack)
			layer->OnDetach();

		LF_OVERSPECIFY("Destroying application named \"{0}\"", m_Spec.Name);

		m_RenderContext->Shutdown();

		LF_OVERSPECIFY("Finished destruction of application named \"{0}\"", m_Spec.Name);
	}
}