#pragma once

namespace LoFox {

	enum EventType {

		None = 0,
		WindowClose, WindowResize,
		KeyPressed, KeyReleased, KeyTyped,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
		FramebufferResize
	};

	enum EventCategory {

		EventCategoryNone = 0,
		EventCategoryApplication = BIT(0),
		EventCategoryInput = BIT(1),
		EventCategoryKeyboard = BIT(2),
		EventCategoryMouse = BIT(3),
		EventCategoryMouseButton = BIT(4)
	};

	class Event {

	public:
		virtual ~Event() = default;

		bool Handled = false;

		virtual EventType GetEventType() const = 0;
		virtual int GetCategoryFlags() const = 0;

		inline bool IsInCategory(EventCategory category) { return GetCategoryFlags() & category; }
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