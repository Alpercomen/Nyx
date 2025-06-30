#pragma once
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#include <Application/Window/Window.h>
#include <Application/Constants/Constants.h>

#include <Application/Resource/Components/Components.h>
#include <Application/Utils/ShaderUtils/ShaderUtils.h>
#include <Application/Utils/SpaceUtils/SpaceUtils.h>
#include <Application/Core/Managers/EntityManager/EntityManager.h>

#include <Application/Core/Engine/Engine.h>
#include <Application/Core/Input/InputDispatcher.h>
#include <Application/Core/Managers/SceneManager/SceneManager.h>

using namespace Nyx;

int main()
{
    BasicWindow window;
    Engine engine(window.GetHandle());
    SceneManager sceneManager;
    SceneID sceneID = sceneManager.CreateScene();
    Scene& scene = *sceneManager.GetActiveScene();

    sceneManager.GenerateEntities(sceneID);

    window.Show();
    while(window.IsActive())
    {
        window.PollEvents();
        engine.Present(scene);
        window.SwapBuffers();
    }
    window.Hide();

    return 0;
}