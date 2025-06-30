#pragma once
#include <Application/Core/Core.h>
#include <Application/Core/Input/InputDispatcher.h>
#include <Application/Core/Input/InputQueue.h>

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

	struct BasicWindowDesc 
	{
		String name = "Nyx";
		Math::Vec2u windowSize = {1280, 720};
		WindowMode windowMode = WindowMode::WINDOWED_MODE;
	};

	struct InputHelper
	{
		static void SetMouseMode(MouseMode mode);
		static MouseMode GetMouseMode();

		static void ProcessMouseButtons();
		static void ProcessMouseMovement();
	};

	struct InputCallbacks
	{
		static void MouseButtonCallback(GLFWwindow* window, int32 button, int32 action, int32 mods);
		static void CursorPosCallback(GLFWwindow* window, float64 xPos, float64 yPos);
		static void KeyboardCallback(GLFWwindow* window, int32 key, int32 scanCode, int32 action, int32 mods);
	};

	class BasicWindow 
	{
	public:
		BasicWindow();
		BasicWindow(const BasicWindowDesc& windowDesc);

		~BasicWindow();

		void PollEvents();
		void SwapBuffers();
		void Show();
		void Hide();

		bool8 IsHidden() const { return m_isHidden; }
		bool8 IsActive();

		void ProcessKeyboard();

		void* GetHandle(); // remove (it might be required)

		const String& GetName() const { return m_windowDesc.name; }
		const Math::Vec2u& GetWindowSize() const { return m_windowDesc.windowSize; }
		WindowMode GetWindowMode() const { return m_windowDesc.windowMode; }

		void SetName(const String& name);
		void SetWindowSize(const Math::Vec2u& newSize);
		void SetWindowMode(WindowMode newMode);

	private:
		bool8 m_isHidden = true;
		BasicWindowDesc m_windowDesc;
		InputQueue m_inputQueue;
	};
}