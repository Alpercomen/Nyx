#pragma once
#include <string>
#include <GL/glew.h>

#include <Application/Core/Core.h>

uint32 CompileShader(GLenum type, const String& source);
uint32 CreateShaderProgram(const String& vertexPath, const String& fragmentPath);