#include "ShaderUtils.h"

#include <spdlog/spdlog.h>

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
	uint32 shader = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shader, 1, &src, nullptr);
	glCompileShader(shader);

	// Error handling
	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
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

	uint32 program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	// Error handling
	GLint success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(program, 512, nullptr, infoLog);
		spdlog::critical("Shader linking failed: {}", infoLog);
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return program;
}