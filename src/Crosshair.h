#pragma once

#include <glm/glm.hpp>

#include "Shader.h"
#include "Mesh.h"

// Handles rendering of the crosshair
class Crosshair
{
public:
	Crosshair();

	// Render the crosshair with given size (should be adjusted for aspect ratio)
	void Render(glm::vec2 size);

private:
	Shader shader_;
	Mesh cross_;
};