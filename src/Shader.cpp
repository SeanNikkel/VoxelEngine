#include "Shader.h"
#include <fstream>
#include <sstream>

Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath)
{
	// Retrieve the vertex/fragment source code from the filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;

	// Make ifstream objects throw exceptions
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// Open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e)
	{
		assert(false);
	}
	const char *vShaderCode = vertexCode.c_str();
	const char *fShaderCode = fragmentCode.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	// vertex Shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	// print compile errors if any
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		assert(false);
	};

	// similiar for Fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	// print compile errors if any
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		assert(false);
	};

	// shader Program
	id_ = glCreateProgram();
	glAttachShader(id_, vertex);
	glAttachShader(id_, fragment);
	glLinkProgram(id_);
	// print linking errors if any
	glGetProgramiv(id_, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id_, 512, NULL, infoLog);
		assert(false);
	}

	// delete the shaders as they're linked into our program now and no longer necessery
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