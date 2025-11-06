#pragma once
#include <iostream>

#include <Application/Core/Core.h>
#include <Application/Constants/Constants.h>

#include <Application/Utils/ShaderUtils/ShaderUtils.h>
#include <Application/Utils/SpaceUtils/SpaceUtils.h>
#include <Application/Resource/Components/Components.h>
#include <Application/Core/Services/Managers/EntityManager/EntityManager.h>
#include <Application/Core/Services/Input/InputDispatcher.h>
#include <Application/Core/Services/Managers/SceneManager/SceneManager.h>
#include <Application/Core/Services/ResourceLocator/ResourceLocator.h>
#include <Application/Core/Engine/Engine.h>
#include <Application/Utils/QTUtils/QTUtils.h>

using namespace Nyx;

int main(int argc, char** argv)
{
    ResourceLocator::Initialize(argv[0]);

    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}