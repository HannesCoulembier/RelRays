#pragma once

#include "LoFox/Events/Event.h"
#include "LoFox/Core/MouseCodes.h"

namespace LoFox {

	class MouseMovedEvent : public Event {

	public:
		MouseMovedEvent(float x, float y)
			: m_MouseX(x), m_MouseY(y) {}

		inline float GetX() const { return m_MouseX; }
		inline float GetY() const { return m_MouseY; }

		virtual EventType GetEventType() const override { return EventType::MouseMoved; }
		static EventType GetStaticEventType() { return EventType::MouseMoved; }
	private:
		float m_MouseX, m_MouseY;
	};

	class MouseScrolledEvent : public Event {

	public:
		MouseScrolledEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset), m_YOffset(yOffset) {}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_YOffset; }

		virtual EventType GetEventType() const override { return EventType::MouseScrolled; }
		static EventType GetStaticEventType() { return EventType::MouseScrolled; }
	private:
		float m_XOffset, m_YOffset;
	};

	class MouseButtonEvent : public Event {

	public:
		inline MouseCode GetMouseButton() const { return m_Button; }
	protected:
		MouseButtonEvent(MouseCode button)
			: m_Button(button) {}

		MouseCode m_Button;
	};

	class MouseButtonPressedEvent : public MouseButtonEvent {

	public:
		MouseButtonPressedEvent(MouseCode button)
			: MouseButtonEvent(button) {}

		virtual EventType GetEventType() const override { return EventType::MouseButtonPressed; }
		static EventType GetStaticEventType() { return EventType::MouseButtonPressed; }
	};

	class MouseButtonReleasedEvent : public MouseButtonEvent {

	public:
		MouseButtonReleasedEvent(MouseCode button)
			: MouseButtonEvent(button) {}

		virtual EventType GetEventType() const override { return EventType::MouseButtonReleased; }
		static EventType GetStaticEventType() { return EventType::MouseButtonReleased; }
	};
}