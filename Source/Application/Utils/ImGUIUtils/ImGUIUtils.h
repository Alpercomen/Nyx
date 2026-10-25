#pragma once

#include <Application/Resource/Components/Camera/Camera.h>
#include <Application/Core/Engine/Engine.h>
#include <Application/Resource/Components/Components.h>

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_internal.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

using namespace Nyx;

namespace ImGUIUtils
{
	void Initialize(void* window);

	void InitDockableWindow();

	ImVec2 DrawGameWindow(Engine* engine);

	void DrawSimulationControl(Engine* engine);

	void DrawHierarchy();

	void DrawName(EntityID& id);

	void DrawTransform(EntityID& id);

	void DrawRigidbody(EntityID& id);

	void DrawAtmosphereComponent(EntityID& id);

	void DrawOrbitalParameters(EntityID& id);

	void DrawOrbitTrail(EntityID& id);

	void DragVec3WithMagnitude(const char* label, const char* id, float64* v, float32 speed, const char* unit, float64 magnitude, const char* fmt = "%.10f");

	void DragVec3Colored(const char* label, const char* id, float64* v, float32 speed, const char* unit, float64 magnitude, const char* fmt = "%.10f");

	void DrawInspector();

	void DrawWindow(Engine* enginePtr, Scene* scenePtr);
}


