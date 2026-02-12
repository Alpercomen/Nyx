#pragma once
#include "ShaderUtils.h"

#include <Application/Core/Services/Managers/RenderManager/OpenGL.h>

using namespace Nyx;

static String ReadFile(const String& filePath)
{
	IfStream file(filePath);
	if (!file.is_open())
	{
		spdlog::critical("Failed to open shader file: {}", filePath);
		return "";
	}

	StringStream ss;
	ss << file.rdbuf();
	return ss.str();
}

static uint32 CompileShader(GLenum type, const String& source)
{
	uint32 shader = GL::Get()->glCreateShader(type);
	const char* src = source.c_str();
	GL::Get()->glShaderSource(shader, 1, &src, nullptr);
	GL::Get()->glCompileShader(shader);

	// Error handling
	GLint success;
	GL::Get()->glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		GL::Get()->glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		spdlog::critical("Shader compilation failed: {}", infoLog);
	}

	return shader;
}

uint32 CreateShaderProgram(const String& vertexPath, const String& fragmentPath)
{
	String vertexSource = ReadFile(vertexPath);
	String fragmentSource = ReadFile(fragmentPath);

	uint32 vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
	uint32 fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	uint32 program = GL::Get()->glCreateProgram();
	GL::Get()->glAttachShader(program, vertexShader);
	GL::Get()->glAttachShader(program, fragmentShader);
	GL::Get()->glLinkProgram(program);

	// Error handling
	GLint success;
	GL::Get()->glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		GL::Get()->glGetProgramInfoLog(program, 512, nullptr, infoLog);
		spdlog::critical("Shader linking failed: {}", infoLog);
	}

	GL::Get()->glDeleteShader(vertexShader);
	GL::Get()->glDeleteShader(fragmentShader);

	return program;
}