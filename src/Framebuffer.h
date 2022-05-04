#pragma once

#include "Texture.h"

#include <vector>
#include <memory>

#include <glm/glm.hpp>

// General purpose framebuffer wrapper class
class Framebuffer
{
public:
	Framebuffer(int width, int height);	// Create framebuffer with width and height
	Framebuffer(Framebuffer &&other);	// Move
	void BindFramebuffer();				// Bind the framebuffer for drawing
	Texture &GetTexture();				// Get the texture that was drawn to (depth)
	~Framebuffer();

private:
	GLuint fbo_;
	Texture depthBuffer_;
	int width_, height_;
};

