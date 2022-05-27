#include "Shader.h"
#include <fstream>
#include <sstream>

Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath, const GLchar *geometryPath)
{
	// Create the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	std::string vertexShaderCode = GetShaderCode(vertexPath);
	CompileShader(vertexShader, vertexShaderCode.c_str());

	// Create the fragment shader
	std::string fragmentShaderCode = GetShaderCode(fragmentPath);
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	CompileShader(fragmentShader, fragmentShaderCode.c_str());

	// Create the geometry shader if present
	GLuint geometryShader = 0;
	if (geometryPath != nullptr)
	{
		std::string geometryShaderCode = GetShaderCode(geometryPath);
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		CompileShader(geometryShader, geometryShaderCode.c_str());
	}

	// Link the shaders
	std::vector<GLuint> shaders = { vertexShader, fragmentShader };
	if (geometryShader != 0) shaders.push_back(geometryShader);
	id_ = LinkShaders(shaders);
}

std::string Shader::GetShaderCode(const char *path)
{
	std::string shaderCode;

	// Open for read
	std::ifstream shaderStream(path, std::ios::in);

	assert(shaderStream.is_open());

	std::string line;

	// Read all lines
	while (getline(shaderStream, line))
		shaderCode += "\n" + line;

	shaderStream.close();

	return shaderCode;
}

void Shader::CompileShader(GLuint shaderID, const char *content)
{
	// Load version
	static std::string version;
	if (version.size() == 0)
		version = GetShaderCode("shaders/version.glsl");

	// Load shared definitions
	static std::string shared;
	if (shared.size() == 0)
		shared = GetShaderCode("shaders/Shared.h");

	const char *sources[] = { version.c_str(), shared.c_str(), content };
	glShaderSource(shaderID, GLsizei(std::size(sources)), sources, nullptr);
	glCompileShader(shaderID);

	// Check compilation
	GLint result = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	int logLength;
	glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		std::vector<char> errorMessage(logLength + 1);
		glGetShaderInfoLog(shaderID, logLength, nullptr, &errorMessage[0]);
		printf("%s\n", &errorMessage[0]);
		__debugbreak();
	}
}

GLuint Shader::LinkShaders(const std::vector<GLuint> &shaders)
{
	// Create program and link

	GLuint program = glCreateProgram();

	for (GLuint i : shaders)
		glAttachShader(program, i);

	glLinkProgram(program);

	// Check linking

	GLint result = GL_FALSE;
	glGetProgramiv(program, GL_LINK_STATUS, &result);
	int logLength;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
	if (logLength > 0)
	{
		std::vector<char> errorMessage(logLength + 1);
		glGetProgramInfoLog(program, logLength, nullptr, &errorMessage[0]);
		printf("%s\n", &errorMessage[0]);
		__debugbreak();
	}
	
	// Cleanup

	for (GLuint i : shaders)
		glDetachShader(program, i);

	for (GLuint i : shaders)
		glDeleteShader(i);

	return program;
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

void Shader::SetVar(const char *name, const glm::vec2 &value) const
{
	Use();
	glUniform2f(glGetUniformLocation(id_, name), value.x, value.y);
}

void Shader::SetVar(const char *name, const glm::vec3 &value) const
{
	Use();
	glUniform3f(glGetUniformLocation(id_, name), value.x, value.y, value.z);
}

void Shader::SetVar(const char *name, const glm::mat3 &value) const
{
	Use();
	glUniformMatrix3fv(glGetUniformLocation(id_, name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::SetVar(const char *name, const glm::mat4 &value) const
{
	Use();
	glUniformMatrix4fv(glGetUniformLocation(id_, name), 1, GL_FALSE, glm::value_ptr(value));
}