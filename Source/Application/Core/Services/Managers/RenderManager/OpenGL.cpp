#pragma once
#include "OpenGL.h"

namespace Nyx
{
	thread_local QOpenGLExtraFunctions* GL::s_funcs = nullptr;
}