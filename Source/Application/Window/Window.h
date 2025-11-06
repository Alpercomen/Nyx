#pragma once
#include <Application/Core/Services/Input/InputDispatcher.h>
#include <Application/Core/Services/Input/InputQueue.h>

struct GLFWwindow;

namespace Nyx
{
	extern bool firstMouse;
	extern float lastX;
	extern float lastY;

	enum class MouseMode
	{
		HIDDEN,
		VISIBLE
	};

	enum class WindowMode 
	{
		WINDOWED_MODE,
		BORDERLESS_MODE,
		EXCLUSIVE_MODE
	};

	struct InputHelper
	{
		static void ProcessMouseMovement();
		static void ProcessMouseScroll();
	};
}