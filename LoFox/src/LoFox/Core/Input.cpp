#include "lfpch.h"
#include "LoFox/Core/Input.h"

#include "LoFox/Core/Application.h"

#include "LoFox/Core/Keyboards.h"

#include <GLFW/glfw3.h>

namespace LoFox {

	Keyboard Input::m_Keyboard = Keyboard::BelgianPeriod;

	bool Input::IsKeyPressed(KeyCode key, bool exact) {

		auto* window = static_cast<GLFWwindow*>(Application::GetInstance().GetActiveWindow()->GetWindowHandle());
		int glfwKey = KeyCodeToglfw(key);
		auto state = glfwGetKey(window, (glfwKey));
		if (exact) {
			if (KeyCodeRequiresRightAlt(key) != Input::IsKeyPressed(KeyCode::RightAlt))
				return false;
			if (KeyCodeRequiresShift(key) != (Input::IsKeyPressed(KeyCode::LeftShift) || Input::IsKeyPressed(KeyCode::RightShift)))
				return false;
		}
		return state == GLFW_PRESS;
	}

	// Only returns true when KeyCodes are exactly equal
	bool Input::IsKeyEqualTo(KeyCode a, KeyCode b) {

		return a == b;
	}

	// Returns true f.e. when comparing 'a' and 'A' (keyboard dependent (Belgian period example: ? = ,))
	bool Input::IsKeyEquivalentTo(KeyCode a, KeyCode b) {

		int a_glfw = Input::KeyCodeToglfw(a);
		int b_glfw = Input::KeyCodeToglfw(b);
		bool result = a_glfw == b_glfw; // TODO: Investigate what this line is doing???
		return a_glfw == b_glfw;
	}

	bool Input::IsMouseButtonPressed(MouseCode button) {

		auto* window = static_cast<GLFWwindow*>(Application::GetInstance().GetActiveWindow()->GetWindowHandle());
		auto state = glfwGetMouseButton(window, static_cast<int32_t>(button));
		return state == GLFW_PRESS;
	}

	glm::vec2 Input::GetMousePosition() {

		auto* window = static_cast<GLFWwindow*>(Application::GetInstance().GetActiveWindow()->GetWindowHandle());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX() {

		return GetMousePosition().x;
	}

	float Input::GetMouseY() {

		return GetMousePosition().y;
	}

	void Input::SetKeyboard(Keyboard keyboard) {

		m_Keyboard = keyboard;
	}

	// Maps a Key(Code) like 'A' to the corresponding glfw keycode encoding its position on a keyboard.
	int Input::KeyCodeToglfw(KeyCode key) {

		switch (m_Keyboard) {

		case Keyboard::QWERTY:			return QWERTYKeyboard::KeyCodeToglfw(key);
		case Keyboard::BelgianPeriod:	return BelgianPeriodKeyboard::KeyCodeToglfw(key);
		}

		LF_CORE_ASSERT(false, "Unknown Keyboard!");
		return 0;
	}

	// Maps glfw keycode (dependant on the place on the keyboard) to the corresponding KeyCode using the currently used keyboard.
	KeyCode Input::glfwToKeyCode(int key) {

		switch (m_Keyboard) {

		case Keyboard::QWERTY:			return QWERTYKeyboard::glfwToKeyCode(key);
		case Keyboard::BelgianPeriod:	return BelgianPeriodKeyboard::glfwToKeyCode(key);
		}

		LF_CORE_ASSERT(false, "Unknown Keyboard!");
		return (KeyCode)0;
	}

	// Returns true for stuff like 'A' and false for stuff like 'a'.
	bool Input::KeyCodeRequiresShift(KeyCode key) {

		switch (m_Keyboard) {
		case Keyboard::QWERTY:			return QWERTYKeyboard::KeyCodeRequiresShift(key);
		case Keyboard::BelgianPeriod:	return BelgianPeriodKeyboard::KeyCodeRequiresShift(key);
		}

		LF_CORE_ASSERT(false, "Unknown Keyboard!");
		return false;
	}

	// See KeyCodeRequiresShift (example on Belgian period: @ -> true, r -> false).
	bool Input::KeyCodeRequiresRightAlt(KeyCode key) {

		switch (m_Keyboard) {
		case Keyboard::QWERTY:			return QWERTYKeyboard::KeyCodeRequiresRightAlt(key);
		case Keyboard::BelgianPeriod:	return BelgianPeriodKeyboard::KeyCodeRequiresRightAlt(key);
		}

		LF_CORE_ASSERT(false, "Unknown Keyboard!");
		return false;
	}
}
