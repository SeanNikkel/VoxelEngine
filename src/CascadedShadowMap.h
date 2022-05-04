#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Framebuffer.h"
#include "Shader.h"

// Info for each cascade
struct Cascade
{
	int resolution = 1024;
	float depth = 1.0f;
};

// Info needed by shader to render cascade
struct CascadeShaderInfo
{
	glm::mat4 transform;
	float depth;
	Texture *tex;
};

// Handles generation of large-scale directional light shadows
class CascadedShadowMap
{
public:
	CascadedShadowMap(const std::vector<Cascade> &cascades);

	// Draw all chunks to shadow maps
	void Render(const glm::mat4 &cameraMatrix);

	// Retrieves info to be sent to shaders
	const std::vector<CascadeShaderInfo> &GetShaderInfo();

private:
	std::vector<Cascade> cascades_;
	std::vector<Framebuffer> framebuffers_;
	Shader shader_;
	std::vector<CascadeShaderInfo> shaderInfo_;
};