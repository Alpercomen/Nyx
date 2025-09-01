#pragma once
#include "Window.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <Application/Constants/Constants.h>
#include <Application/Utils/ImGUIUtils/ImGUIUtils.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>

namespace Nyx
{
    GLFWwindow* gWindow = nullptr;
    static InputEvent event;

    bool firstMouse = true;
    float lastX = 400.0f;
    float lastY = 400.f;

    void InputHelper::SetMouseMode(MouseMode mode)
    {
        if (gWindow == nullptr)
            return;

        switch (mode)
        {
        case MouseMode::HIDDEN:
            spdlog::info("Camera is hidden");
            glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
        case MouseMode::VISIBLE:
            spdlog::info("Camera is visible");
            glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        default:
            break;
        }
    }

    MouseMode InputHelper::GetMouseMode()
    {
        if (gWindow == nullptr)
            return MouseMode::HIDDEN;

        switch (glfwGetInputMode(gWindow, GLFW_CURSOR))
        {
        case GLFW_CURSOR_DISABLED:
            return MouseMode::HIDDEN;
        case GLFW_CURSOR_NORMAL:
            return MouseMode::VISIBLE;
        default:
            return MouseMode::VISIBLE;
        }
    }

    void InputHelper::ProcessMouseButtons()
    {
        InputEventDispatcher::Get().AddCallback(EventType::MOUSE_PRESS, [&](const InputEvent& event)
            {
                if (HasFlag(event.m_eventList.mouseButton, MouseButton::RIGHT_BUTTON))
                    SetMouseMode(MouseMode::HIDDEN);
            }
        );
    }

    void InputHelper::ProcessMouseMovement()
    {

        InputEventDispatcher::Get().AddCallback(EventType::MOUSE_MOVE, [&](const InputEvent& event)
            {
                auto& cameraIDs = ECS::Get().GetAllComponentIDs<Camera>();

                if (cameraIDs.size() <= 0)
                    return;

                const EntityID& id = cameraIDs[0];

                auto& camera = *ECS::Get().GetComponent<Camera>(id);

                if (GetMouseMode() != MouseMode::HIDDEN)
                    return;

                double xPos = event.m_eventList.mouseX;
                double yPos = event.m_eventList.mouseY;

                if (firstMouse)
                {
                    lastX = (float)xPos;
                    lastY = (float)yPos;
                    firstMouse = false;
                }

                float xoffset = (float)xPos - lastX;
                float yoffset = lastY - (float)yPos; // reversed y

                lastX = (float)xPos;
                lastY = (float)yPos;

                camera.ProcessMouseMovement(xoffset, yoffset);
            }
        );

    }

    void InputCallbacks::MouseButtonCallback(GLFWwindow* window, int32 button, int32 action, int32 mods)
    {
        InputQueue* queue = static_cast<InputQueue*>(glfwGetWindowUserPointer(gWindow));

        if (action == GLFW_PRESS)
        {
            event.m_eventType = EventType::MOUSE_PRESS;
            event.m_eventList.mouseButton |= static_cast<MouseButton>(1 << button);
        }
        else if (action == GLFW_RELEASE)
        {
            if (HasFlag(event.m_eventList.mouseButton, MouseButton::RIGHT_BUTTON))
            {
                InputHelper::SetMouseMode(MouseMode::VISIBLE);
                firstMouse = true;
            }

            event.m_eventType = EventType::MOUSE_RELEASE;
            event.m_eventList.mouseButton &= ~static_cast<MouseButton>(1 << button);
        }

        queue->PushEvent(event);

        InputEvent newEvent;
        while (queue->PopEvent(newEvent))
            InputEventDispatcher::Get().DispatchCallback(newEvent);
        
    }

    void InputCallbacks::CursorPosCallback(GLFWwindow* window, float64 xPos, float64 yPos)
    {
        InputQueue* queue = static_cast<InputQueue*>(glfwGetWindowUserPointer(window));
        InputEvent localEvent;

        if (!HasFlag(event.m_eventList.mouseButton, MouseButton::RIGHT_BUTTON))
            return;

        localEvent.m_eventType = EventType::MOUSE_MOVE;
        localEvent.m_eventList.mouseX = static_cast<int32>(xPos);
        localEvent.m_eventList.mouseY = static_cast<int32>(yPos);

        queue->PushEvent(localEvent);

        InputEvent newEvent;
        while (queue->PopEvent(newEvent))
            InputEventDispatcher::Get().DispatchCallback(newEvent);
    }

    void InputCallbacks::KeyboardCallback(GLFWwindow* window, int32 key, int32 scanCode, int32 action, int32 mods) 
    {
        InputQueue* queue = static_cast<InputQueue*>(glfwGetWindowUserPointer(window));
        InputEvent localEvent;

        if (action == GLFW_PRESS)
        {
            localEvent.m_eventType = EventType::KEY_PRESS;
            localEvent.m_eventList.keyCode = key;
        }
        else if (action == GLFW_RELEASE)
        {
            localEvent.m_eventType = EventType::KEY_RELEASE;
            localEvent.m_eventList.keyCode = key;
        }

        queue->PushEvent(localEvent);

        InputEvent newEvent;
        while (queue->PopEvent(newEvent))
            InputEventDispatcher::Get().DispatchCallback(newEvent);
    }

    BasicWindow::BasicWindow() : BasicWindow(BasicWindowDesc()) {}

    BasicWindow::BasicWindow(const BasicWindowDesc& windowDesc) : m_windowDesc(windowDesc)
    {
        if (!glfwInit())
        {
            spdlog::critical("Failed to initialize GLFW");
            exit(-1);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

        gWindow = glfwCreateWindow(int32(m_windowDesc.windowSize.x), int32(m_windowDesc.windowSize.y), m_windowDesc.name.data(), NULL, NULL);

        if (!gWindow)
        {
            spdlog::critical("Failed to create GLFW window");
            glfwTerminate();
            exit(-1);
        }

        glfwSetWindowUserPointer(gWindow, &m_inputQueue);
        glfwMakeContextCurrent(gWindow); // Ensure OpenGL context is active before returning

        glfwSetMouseButtonCallback(gWindow, InputCallbacks::MouseButtonCallback);
        glfwSetCursorPosCallback(gWindow, InputCallbacks::CursorPosCallback);
        glfwSetKeyCallback(gWindow, InputCallbacks::KeyboardCallback);

        glewExperimental = GL_TRUE;
        glewInit();

        ImGUIUtils::Initialize(gWindow);
    }

    BasicWindow::~BasicWindow()
    {
        if (gWindow)
        {
            glfwSetWindowShouldClose(gWindow, true);
            glfwDestroyWindow(gWindow);
            glfwTerminate();

            gWindow = nullptr;
        }
    }

    void BasicWindow::PollEvents()
    {
        ProcessKeyboard();
        glfwPollEvents();
    }

    void BasicWindow::SwapBuffers()
    {
        glfwSwapBuffers(gWindow);
    }

    void BasicWindow::Show()
    {
        glfwShowWindow(gWindow);
    }

    void BasicWindow::Hide()
    {
        glfwHideWindow(gWindow);
    }

    bool8 BasicWindow::IsActive()
    {
        return !glfwWindowShouldClose(gWindow);
    }

    void BasicWindow::ProcessKeyboard()
    {
        if (InputHelper::GetMouseMode() != MouseMode::HIDDEN)
            return;

        if (gWindow == nullptr)
            return;

        auto& cameraIDs = ECS::Get().GetAllComponentIDs<Camera>();

        if (cameraIDs.size() <= 0)
            return;

        const EntityID& id = cameraIDs[0];
        auto& camera = *ECS::Get().GetComponent<Camera>(id);

        float effectiveDelta = DELTA_TIME;
        if (glfwGetKey(gWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
            effectiveDelta *= camera.GetMovementSpeedMultiplier();

        if (glfwGetKey(gWindow, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
            effectiveDelta /= camera.GetMovementSpeedMultiplier();

        if (glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboardMovement(FORWARD, effectiveDelta);
        if (glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboardMovement(BACKWARD, effectiveDelta);
        if (glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboardMovement(LEFT, effectiveDelta);
        if (glfwGetKey(gWindow, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboardMovement(RIGHT, effectiveDelta);
        if (glfwGetKey(gWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
            camera.ProcessKeyboardMovement(UP, effectiveDelta);
        if (glfwGetKey(gWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            camera.ProcessKeyboardMovement(DOWN, effectiveDelta);
    }

    void* BasicWindow::GetHandle()
    {
        return gWindow;
    }

    void BasicWindow::SetName(const String& name)
    {
        if (gWindow)
        {
            glfwSetWindowTitle(gWindow, name.data());
            m_windowDesc.name = name;
        }
    }

    void BasicWindow::SetWindowSize(const Math::Vec2u& newSize)
    {
        if (gWindow)
        {
            glfwSetWindowSize(gWindow, newSize.x, newSize.y);
            m_windowDesc.windowSize = newSize;
        }
    }

    void BasicWindow::SetWindowMode(WindowMode newMode)
    {
        // Soon...
    }
}

