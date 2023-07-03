#pragma once

#include "LoFox/Events/Event.h"
#include "LoFox/Core/Core.h"

namespace LoFox {

	class FramebufferResizeEvent : public Event {

	public:
		FramebufferResizeEvent(uint32_t width, uint32_t height)
			: m_Width(width), m_Height(height) {}

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }

		virtual EventType GetEventType() const override { return EventType::FramebufferResize; }
		static EventType GetStaticEventType() { return EventType::FramebufferResize; }
	private:
		uint32_t m_Width, m_Height;
	};
}