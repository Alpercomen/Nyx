#pragma once
#include <Application/Core/Core.h>

namespace Nyx {
	enum class MouseButton {
		NONE = 0,
		LEFT_BUTTON = 1 << 0,
		RIGHT_BUTTON = 1 << 1,
		MIDDLE_BUTTON = 1 << 2,
		ALL = LEFT_BUTTON | RIGHT_BUTTON | MIDDLE_BUTTON,
	};

	enum class EventType {
		NONE,
		MOUSE_PRESS,
		MOUSE_RELEASE,
		MOUSE_MOVE,
		MOUSE_SCROLL_WHEEL,
		MOUSE_DOUBLE_CLICK,
		KEY_PRESS,
		KEY_RELEASE,
		KEY_CHAR
	};

	struct EventList {
		int32 keyCode;
		int8 inputChar;
		int32 mouseX;
		int32 mouseY;
		MouseButton mouseButton;
		float32 scrollDelta;
	};

	struct InputEvent {
		InputEvent (EventType eventType = EventType::NONE) : m_eventType(eventType) {}
		~InputEvent() = default;

		EventList m_eventList;
		EventType m_eventType;
	};
}