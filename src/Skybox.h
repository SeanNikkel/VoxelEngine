#pragma once

#include <glm/glm.hpp>

#include "Shader.h"
#include "Mesh.h"

// Handles rendering of the skybox
class Skybox
{
public:
	Skybox();
	
	// Render skybox on top of screen without modifying depth buffer
	void Render(const glm::mat4 &viewMatrix, const glm::mat4 &projMatrix);

private:
	Shader shader_;
	Mesh box_;
};