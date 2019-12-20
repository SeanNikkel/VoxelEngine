#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <algorithm>

Texture::Texture(std::string path, bool alphaChannel, bool mipmap, int wrapMode, int filter)
{
	glGenTextures(1, &id_);
	glBindTexture(GL_TEXTURE_2D, id_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	stbi_set_flip_vertically_on_load(true);
	unsigned char *data = stbi_load(path.c_str(), &width_, &height_, &channels_, 0);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, alphaChannel ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);

		if (mipmap)
			glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		assert(!path.c_str());
	}

	stbi_image_free(data);
}

int Texture::GetWidth() const
{
	return width_;
}

int Texture::GetHeight() const
{
	return height_;
}

int Texture::GetChannelCount() const
{
	return channels_;
}

void Texture::Activate(GLenum texture)
{
	glActiveTexture(texture);
	glBindTexture(GL_TEXTURE_2D, id_);
}
