#pragma once
#include <Application/Core/Services/Input/InputDispatcher.h>
#include <Application/Constants/Constants.h>
#include <Application/Core/Core.h>

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

