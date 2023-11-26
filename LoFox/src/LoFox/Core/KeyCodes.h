#pragma once

#include "LoFox/Core/Core.h"

namespace LoFox {

	typedef enum class KeyCode {

		// ASCII
		Space = 32, /* space */
		ExclamationMark = 33, /* ! */
		DoubleQuotes = 34, /* " */
		Hash = 35, /* # */
		DollarSign = 36, /* # */
		Percent = 37, /* % */
		Ampersand = 38, /* & */
		Apostrophe = 39, /* ' */
		LeftParenthesis = 40, /* ( */
		RightParenthesis = 41, /* ) */
		Asterix = 42, /* * */
		Plus = 43, /* + */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Colon = 58, /* : */
		Semicolon = 59, /* ; */
		LeftAngledBracket = 60, /* < */
		Equal = 61, /* = */
		RightAngledBracket = 62, /* > */
		QuestionMark = 63, /* ? */
		AtSign = 64, /* @ */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftSquareBracket = 91, /* [ */
		Backslash = 92, /* \ */
		RightSquareBracket = 93, /* ] */
		Caret = 94, /* ^ */
		Underscore = 95, /* _ */
		GraveAccent = 96, /* ` */

		// glfw outputs only capital letters, but this is still handy
		a = 97,
		b = 98,
		c = 99,
		d = 100,
		e = 101,
		f = 102,
		g = 103,
		h = 104,
		i = 105,
		j = 106,
		k = 107,
		l = 108,
		m = 109,
		n = 110,
		o = 111,
		p = 112,
		q = 113,
		r = 114,
		s = 115,
		t = 116,
		u = 117,
		v = 118,
		w = 119,
		x = 120,
		y = 121,
		z = 122,

		LeftCurlyBracket = 123, /* { */
		VerticalBar = 124, /* | */
		RightCurlyBracket = 125, /* } */
		Tilde = 126, /* ~ */

		// Selection of extended ASCII from https://www.ascii-code.com/
		EuroSign = 128, /* € */
		SmallLigatureOe = 156, /* œ */
		PoundSign = 163, /* £ */
		SectionSign = 167, /* § */
		Umlaut = 168, /* ¨ */
		DegreeSign = 176, /* ° */
		SuperscriptTwo = 178, /* ² */
		SuperscriptThree = 179, /* ³ */
		AcuteAccent = 180, /* ´ */
		MicroSign = 181, /* µ */
		aGrave = 192, /* à */
		cCedilla = 199, /* ç */
		eGrave = 200, /* è */
		eAcute = 201, /* é */
		uGrave = 217, /* ù */

		// Function Keys (extended ASCII goes up to 255, so everything above is fine)
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348,

		Unknown = -1,	/* means it's not possible to get that keycode on the current keyboard layout */

	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode) {

		os << static_cast<int32_t>(keyCode);
		return os;
	}
}


#define LF_KEY_SPACE              ::LoFox::Key::Space
#define LF_KEY_APOSTROPHE         ::LoFox::Key::Apostrophe  /* ' */
#define LF_KEY_COMMA              ::LoFox::Key::Comma		/* , */
#define LF_KEY_MINUS              ::LoFox::Key::Minus		/* - */
#define LF_KEY_PERIOD             ::LoFox::Key::Period		/* . */
#define LF_KEY_SLASH              ::LoFox::Key::Slash		/* / */
#define LF_KEY_0                  ::LoFox::Key::D0
#define LF_KEY_1                  ::LoFox::Key::D1
#define LF_KEY_2                  ::LoFox::Key::D2
#define LF_KEY_3                  ::LoFox::Key::D3
#define LF_KEY_4                  ::LoFox::Key::D4
#define LF_KEY_5                  ::LoFox::Key::D5
#define LF_KEY_6                  ::LoFox::Key::D6
#define LF_KEY_7                  ::LoFox::Key::D7
#define LF_KEY_8                  ::LoFox::Key::D8
#define LF_KEY_9                  ::LoFox::Key::D9
#define LF_KEY_SEMICOLON          ::LoFox::Key::Semicolon	/* ; */
#define LF_KEY_EQUAL              ::LoFox::Key::Equal		/* = */
#define LF_KEY_A                  ::LoFox::Key::A
#define LF_KEY_B                  ::LoFox::Key::B
#define LF_KEY_C                  ::LoFox::Key::C
#define LF_KEY_D                  ::LoFox::Key::D
#define LF_KEY_E                  ::LoFox::Key::E
#define LF_KEY_F                  ::LoFox::Key::F
#define LF_KEY_G                  ::LoFox::Key::G
#define LF_KEY_H                  ::LoFox::Key::H
#define LF_KEY_I                  ::LoFox::Key::I
#define LF_KEY_J                  ::LoFox::Key::J
#define LF_KEY_K                  ::LoFox::Key::K
#define LF_KEY_L                  ::LoFox::Key::L
#define LF_KEY_M                  ::LoFox::Key::M
#define LF_KEY_N                  ::LoFox::Key::N
#define LF_KEY_O                  ::LoFox::Key::O
#define LF_KEY_P                  ::LoFox::Key::P
#define LF_KEY_Q                  ::LoFox::Key::Q
#define LF_KEY_R                  ::LoFox::Key::R
#define LF_KEY_S                  ::LoFox::Key::S
#define LF_KEY_T                  ::LoFox::Key::T
#define LF_KEY_U                  ::LoFox::Key::U
#define LF_KEY_V                  ::LoFox::Key::V
#define LF_KEY_W                  ::LoFox::Key::W
#define LF_KEY_X                  ::LoFox::Key::X
#define LF_KEY_Y                  ::LoFox::Key::Y
#define LF_KEY_Z                  ::LoFox::Key::Z
#define LF_KEY_LEFT_BRACKET       ::LoFox::Key::LeftBracket		/* [ */
#define LF_KEY_BACKSLASH          ::LoFox::Key::Backslash		/* \ */
#define LF_KEY_RIGHT_BRACKET      ::LoFox::Key::RightBracket	/* ] */
#define LF_KEY_GRAVE_ACCENT       ::LoFox::Key::GraveAccent		/* ` */
#define LF_KEY_WORLD_1            ::LoFox::Key::World1			/* non-US #1 */
#define LF_KEY_WORLD_2            ::LoFox::Key::World2			/* non-US #2 */

/* Function keys */
#define LF_KEY_ESCAPE             ::LoFox::Key::Escape		
#define LF_KEY_ENTER              ::LoFox::Key::Enter		
#define LF_KEY_TAB                ::LoFox::Key::Tab			
#define LF_KEY_BACKSPACE          ::LoFox::Key::Backspace	
#define LF_KEY_INSERT             ::LoFox::Key::Insert		
#define LF_KEY_DELETE             ::LoFox::Key::Delete		
#define LF_KEY_RIGHT              ::LoFox::Key::Right		
#define LF_KEY_LEFT               ::LoFox::Key::Left		
#define LF_KEY_DOWN               ::LoFox::Key::Down		
#define LF_KEY_UP                 ::LoFox::Key::Up			
#define LF_KEY_PAGE_UP            ::LoFox::Key::PageUp		
#define LF_KEY_PAGE_DOWN          ::LoFox::Key::PageDown	
#define LF_KEY_HOME               ::LoFox::Key::Home		
#define LF_KEY_END                ::LoFox::Key::End			
#define LF_KEY_CAPS_LOCK          ::LoFox::Key::CapsLock	
#define LF_KEY_SCROLL_LOCK        ::LoFox::Key::ScrollLock	
#define LF_KEY_NUM_LOCK           ::LoFox::Key::NumLock		
#define LF_KEY_PRINT_SCREEN       ::LoFox::Key::PrintScreen	
#define LF_KEY_PAUSE              ::LoFox::Key::Pause		
#define LF_KEY_F1                 ::LoFox::Key::F1			
#define LF_KEY_F2                 ::LoFox::Key::F2			
#define LF_KEY_F3                 ::LoFox::Key::F3			
#define LF_KEY_F4                 ::LoFox::Key::F4			
#define LF_KEY_F5                 ::LoFox::Key::F5			
#define LF_KEY_F6                 ::LoFox::Key::F6			
#define LF_KEY_F7                 ::LoFox::Key::F7			
#define LF_KEY_F8                 ::LoFox::Key::F8			
#define LF_KEY_F9                 ::LoFox::Key::F9			
#define LF_KEY_F10                ::LoFox::Key::F10			
#define LF_KEY_F11                ::LoFox::Key::F11			
#define LF_KEY_F12                ::LoFox::Key::F12			
#define LF_KEY_F13                ::LoFox::Key::F13			
#define LF_KEY_F14                ::LoFox::Key::F14			
#define LF_KEY_F15                ::LoFox::Key::F15			
#define LF_KEY_F16                ::LoFox::Key::F16			
#define LF_KEY_F17                ::LoFox::Key::F17			
#define LF_KEY_F18                ::LoFox::Key::F18			
#define LF_KEY_F19                ::LoFox::Key::F19			
#define LF_KEY_F20                ::LoFox::Key::F20			
#define LF_KEY_F21                ::LoFox::Key::F21			
#define LF_KEY_F22                ::LoFox::Key::F22			
#define LF_KEY_F23                ::LoFox::Key::F23			
#define LF_KEY_F24                ::LoFox::Key::F24			
#define LF_KEY_F25                ::LoFox::Key::F25			
#define LF_KEY_KP_0               ::LoFox::Key::KP0			
#define LF_KEY_KP_1               ::LoFox::Key::KP1			
#define LF_KEY_KP_2               ::LoFox::Key::KP2			
#define LF_KEY_KP_3               ::LoFox::Key::KP3			
#define LF_KEY_KP_4               ::LoFox::Key::KP4			
#define LF_KEY_KP_5               ::LoFox::Key::KP5			
#define LF_KEY_KP_6               ::LoFox::Key::KP6			
#define LF_KEY_KP_7               ::LoFox::Key::KP7			
#define LF_KEY_KP_8               ::LoFox::Key::KP8			
#define LF_KEY_KP_9               ::LoFox::Key::KP9			
#define LF_KEY_KP_DECIMAL         ::LoFox::Key::KPDecimal	
#define LF_KEY_KP_DIVIDE          ::LoFox::Key::KPDivide	
#define LF_KEY_KP_MULTIPLY        ::LoFox::Key::KPMultiply	
#define LF_KEY_KP_SUBTRACT        ::LoFox::Key::KPSubtract	
#define LF_KEY_KP_ADD             ::LoFox::Key::KPAdd		
#define LF_KEY_KP_ENTER           ::LoFox::Key::KPEnter		
#define LF_KEY_KP_EQUAL           ::LoFox::Key::KPEqual		
#define LF_KEY_LEFT_SHIFT         ::LoFox::Key::LeftShift	
#define LF_KEY_LEFT_CONTROL       ::LoFox::Key::LeftControl	
#define LF_KEY_LEFT_ALT           ::LoFox::Key::LeftAlt		
#define LF_KEY_LEFT_SUPER         ::LoFox::Key::LeftSuper	
#define LF_KEY_RIGHT_SHIFT        ::LoFox::Key::RightShift	
#define LF_KEY_RIGHT_CONTROL      ::LoFox::Key::RightControl
#define LF_KEY_RIGHT_ALT          ::LoFox::Key::RightAlt	
#define LF_KEY_RIGHT_SUPER        ::LoFox::Key::RightSuper	
#define LF_KEY_MENU               ::LoFox::Key::Menu	

#define LF_KEY_UNKNOWN            ::LoFox::Key::Unknown	
