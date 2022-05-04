#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <algorithm>

Texture::Texture(std::string path, bool alphaChannel, bool mipmap, GLenum wrapMode, GLenum filtering)
{
	// Initialize gpu texture
	glGenTextures(1, &id_);
	glBindTexture(GL_TEXTURE_2D, id_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtering);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtering);

	// Load image pixels
	stbi_set_flip_vertically_on_load(true);
	int channels;
	unsigned char *data = stbi_load(path.c_str(), &width_, &height_, &channels, 0);

	if (data)
	{
		// Send pixel data to texture
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, alphaChannel ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

		// Generate mipmaps
		if (mipmap)
			glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		assert(false); //error
	}

	// Free cpu pixel data
	stbi_image_free(data);
}

Texture::Texture(int width, int height, int arraySize, GLenum format, GLenum wrapMode, GLenum filtering)
	: width_(width), height_(height)
{
	GLenum type = arraySize > 1 ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D;

	// Initialize gpu texture
	glGenTextures(1, &id_);
	glBindTexture(type, id_);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, filtering);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, filtering);
	glTexParameteri(type, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, wrapMode);

	// Allocate gpu memory for texture
	if (type == GL_TEXTURE_2D)
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GetFormat(format), GL_FLOAT, nullptr);
	else
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, format, width, height, arraySize, 0, GetFormat(format), GL_FLOAT, nullptr);
}

Texture::Texture(Texture &&other)
{
	id_ = other.id_;
	width_ = other.width_;
	height_ = other.height_;

	other.id_ = 0;
}

int Texture::GetWidth() const
{
	return width_;
}

int Texture::GetHeight() const
{
	return height_;
}

void Texture::Activate(GLenum texture)
{
	glActiveTexture(texture);
	glBindTexture(GL_TEXTURE_2D, id_);
}

unsigned Texture::GetID() const
{
	return id_;
}

Texture::~Texture()
{
	glDeleteTextures(1, &id_);
}

GLenum Texture::GetFormat(GLenum internalFormat)
{
	switch (internalFormat)
	{
	case GL_RED:
	case GL_R8:
	case GL_R8_SNORM:
	case GL_R16:
	case GL_R16_SNORM:
	case GL_R16F:
	case GL_R32F:
		return GL_RED;
	case GL_RG:
	case GL_RG8:
	case GL_RG8_SNORM:
	case GL_RG16:
	case GL_RG16_SNORM:
	case GL_RG16F:
	case GL_RG32F:
		return GL_RG;
	case GL_RGB:
	case GL_RGB8:
	case GL_RGB8_SNORM:
	case GL_RGB16_SNORM:
	case GL_SRGB8:
	case GL_RGB16F:
	case GL_RGB32F:
		return GL_RGB;
	case GL_RGBA:
	case GL_RGBA8:
	case GL_RGBA8_SNORM:
	case GL_SRGB8_ALPHA8:
	case GL_RGBA16F:
	case GL_RGBA32F:
		return GL_RGBA;
	case GL_DEPTH_COMPONENT:
	case GL_DEPTH_COMPONENT16:
	case GL_DEPTH_COMPONENT24:
	case GL_DEPTH_COMPONENT32F:
		return GL_DEPTH_COMPONENT;
	default:
		assert(!"Undefined format!");
	}
	return 0;
}
