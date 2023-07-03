#pragma once

#include "LoFox/Events/Event.h"
#include "LoFox/Core/KeyCodes.h"

namespace LoFox {

	class KeyEvent : public Event {

	public:
		inline KeyCode GetKeyCode() const { return m_KeyCode; }
	protected:
		KeyEvent(KeyCode keycode)
			: m_KeyCode(keycode) {}

		KeyCode m_KeyCode;
	};

	class KeyPressedEvent : public KeyEvent {

	public:
		KeyPressedEvent(KeyCode keycode, const uint16_t isRepeat)
			: KeyEvent(keycode), m_IsRepeat(isRepeat) {}

		uint16_t IsRepeat() const { return m_IsRepeat; }
		
		virtual EventType GetEventType() const override { return EventType::KeyPressed; }
		static EventType GetStaticEventType() { return EventType::KeyPressed; }
	private:
		bool m_IsRepeat;
	};

	class KeyReleasedEvent : public KeyEvent {

	public:
		KeyReleasedEvent(KeyCode keycode)
			: KeyEvent(keycode) {}

		virtual EventType GetEventType() const override { return EventType::KeyReleased; }
		static EventType GetStaticEventType() { return EventType::KeyReleased; }
	};

	class KeyTypedEvent : public KeyEvent {

	public:
		KeyTypedEvent(KeyCode keycode)
			: KeyEvent(keycode) {}

		virtual EventType GetEventType() const override { return EventType::KeyTyped; }
		static EventType GetStaticEventType() { return EventType::KeyTyped; }
	};
}