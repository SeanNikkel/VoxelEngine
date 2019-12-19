#pragma once

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>


class Shader
{
public:
	// Read and build the shader
	Shader(const GLchar *vertexPath, const GLchar *fragmentPath);

	// Activate the shader
	void Use() const;

	// Uniform functions
	void SetVar(const char *name, bool value) const;
	void SetVar(const char *name, int value) const;
	void SetVar(const char *name, float value) const;
	void SetVar(const char *name, const glm::mat4 &value) const;

private:
	unsigned int id_;
};