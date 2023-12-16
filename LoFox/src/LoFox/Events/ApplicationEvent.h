#pragma once

#include "LoFox/Events/Event.h"
#include "LoFox/Core/Core.h"

namespace LoFox {

	class WindowResizeEvent : public Event {

	public:
		WindowResizeEvent(uint32_t width, uint32_t height)
			: m_Width(width), m_Height(height) {}

		inline uint32_t GetWidth() const { return m_Width; }
		inline uint32_t GetHeight() const { return m_Height; }

		virtual EventType GetEventType() const override { return EventType::WindowResize; }
		static EventType GetStaticEventType() { return EventType::WindowResize; }
		virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
	private:
		uint32_t m_Width, m_Height;
	};

	class WindowCloseEvent : public Event {

	public:
		WindowCloseEvent() {}

		virtual EventType GetEventType() const override { return EventType::WindowClose; }
		static EventType GetStaticEventType() { return EventType::WindowClose; }
		virtual int GetCategoryFlags() const override { return EventCategoryApplication; }
	};
}