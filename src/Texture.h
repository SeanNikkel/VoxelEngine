#pragma once

#include <glad/glad.h>
#include <string>

// Wrapper for graphics api textures
class Texture
{
public:
	// Texture from file
	Texture(std::string path, bool alphaChannel = true, bool mipmap = true, GLenum wrapMode = GL_REPEAT, GLenum filtering = GL_LINEAR);

	// Blank texture
	Texture(int width, int height, int arraySize, GLenum format, GLenum wrapMode = GL_CLAMP_TO_EDGE, GLenum filtering = GL_LINEAR);

	// Move
	Texture(Texture &&other);

	// Getters
	int GetWidth() const;
	int GetHeight() const;
	unsigned GetID() const;

	// Use this texture to draw
	void Activate(GLenum texture = 0);

	// Texture format helper
	static GLenum GetFormat(GLenum internalFormat);

	~Texture();

private:
	unsigned id_;
	int width_;
	int height_;
};