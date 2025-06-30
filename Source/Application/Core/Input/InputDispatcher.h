#pragma once
#include <Application/Core/Core.h>
#include <Application/Core/Input/InputEvent.h>
#include <Application/Resource/Camera/Camera.h>

namespace Nyx {
	// CRTP (Curiously Recursive Templated Programming)
	class InputEventDispatcher : public Singleton<InputEventDispatcher> {
		using EventCallback = function<void(const InputEvent&)>;

	public:
		using MouseButtonCallback = function<void(int32 button, int32 action, int32 mods)>;
		using CursorPosCallback = function<void(float64 x, float64 y)>;
		using KeyboardCallback = function<void(int32 key, int32 scanCode, int32 action, int32 mode)>;

		void AddCallback(EventType eventType, EventCallback callback);
		void DispatchCallback(const InputEvent& inputEvent);

	private:
		HashMap<EventType, Vector<EventCallback>> m_callbacks;
	};
}


