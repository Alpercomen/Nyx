#include <GLFW/glfw3.h>
#include <Application/Core/Input/InputDispatcher.h>
#include <Application/Constants/Constants.h>

namespace Nyx
{
    void InputEventDispatcher::AddCallback(EventType eventType, EventCallback callback)
    {
        m_callbacks[eventType].push_back(callback);
    }

    void InputEventDispatcher::DispatchCallback(const InputEvent& inputEvent)
    {
        if (m_callbacks.find(inputEvent.m_eventType) != m_callbacks.end())
        {
            for (auto& el : m_callbacks[inputEvent.m_eventType])
            {
                el(inputEvent);
            }
        }
    }
}

