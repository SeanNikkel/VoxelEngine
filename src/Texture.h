#pragma once

#include <glad/glad.h>
#include <string>

// Wrapper for graphics api textures
class Texture
{
public:
	Texture(std::string path, bool alphaChannel = true, bool mipmap = true, int wrapMode = GL_REPEAT, int filter = GL_LINEAR);

	// Getters
	int GetWidth() const;
	int GetHeight() const;
	int GetChannelCount() const;

	// Use this texture to draw
	void Activate(GLenum texture = 0);

private:
	unsigned id_;
	int width_;
	int height_;
	int channels_;
};