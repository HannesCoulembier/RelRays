#pragma once

namespace LoFox {

	enum EventType {

		None = 0,
		WindowClose, WindowResize,
		KeyPressed, KeyReleased, KeyTyped, // TODO: implement KeyPressedEvent, KeyReleasedEvent and KeyTypedEvent classes and add callbacks
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled, // TODO: implement MouseButtonPressedEvent, MouseButtonReleasedEvent, MouseMovedEvent and MouseScrolledEvent classes and add callbacks
		FramebufferResize
	};

	class Event {

	public:
		virtual ~Event() = default;

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
	};

	class EventDispatcher {

	public:
		EventDispatcher(Event& event)
			: m_Event(event) {}

		template<typename T, typename F>
		bool Dispatch(F& func) {

			if (m_Event.GetEventType() == T::GetStaticEventType()) {

				m_Event.Handled |= func(static_cast<T&>(m_Event));
				return true;
			}
			return false;
		}
	private:
		Event& m_Event;
	};
}