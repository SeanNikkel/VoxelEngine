#include "Shader.h"
#include <fstream>
#include <sstream>

Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath)
{
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// Make ifstreams throw exceptions
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	// Read in source code
	try
	{
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		// Read files into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		// Close files
		vShaderFile.close();
		fShaderFile.close();

		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		assert(false);
	}
	const char *vShaderCode = vertexCode.c_str();
	const char *fShaderCode = fragmentCode.c_str();

	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// Compile vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	// Error checking
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		assert(false);
	};

	// Compile fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	// Error checking
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		assert(false);
	};

	// Create shader program
	id_ = glCreateProgram();
	glAttachShader(id_, vertex);
	glAttachShader(id_, fragment);
	glLinkProgram(id_);

	// Error checking
	glGetProgramiv(id_, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id_, 512, NULL, infoLog);
		assert(false);
	}

	// Delete the unlinked shaders
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::Use() const
{
	glUseProgram(id_);
}

void Shader::SetVar(const char *name, bool value) const
{
	Use();
	glUniform1i(glGetUniformLocation(id_, name), (int)value);
}

void Shader::SetVar(const char *name, int value) const
{
	Use();
	glUniform1i(glGetUniformLocation(id_, name), value);
}

void Shader::SetVar(const char *name, float value) const
{
	Use();
	glUniform1f(glGetUniformLocation(id_, name), value);
}

void Shader::SetVar(const char *name, const glm::vec3 &value) const
{
	Use();
	glUniform3f(glGetUniformLocation(id_, name), value.x, value.y, value.z);
}

void Shader::SetVar(const char *name, const glm::mat4 &value) const
{
	Use();
	glUniformMatrix4fv(glGetUniformLocation(id_, name), 1, GL_FALSE, glm::value_ptr(value));
}