#include "Framebuffer.h"

#include <cassert>

Framebuffer::Framebuffer(int width, int height)
	: width_(width), height_(height), depthBuffer_(width, height, 1, GL_DEPTH_COMPONENT32F)
{
	// Create the framebuffer
	glGenFramebuffers(1, &fbo_);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

	// Add the depth texture
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer_.GetID(), 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// Error checking
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	// Render to screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Framebuffer::Framebuffer(Framebuffer &&other)
	: depthBuffer_(std::move(other.depthBuffer_))
{
	fbo_ = other.fbo_;
	width_ = other.width_;
	height_ = other.height_;

	other.fbo_ = 0;
}

void Framebuffer::BindFramebuffer()
{
	// Bind the framebuffer for drawing
	glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

	// Set viewport to framebuffer size
	glViewport(0, 0, width_, height_);

	// Clear
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_DEPTH_BUFFER_BIT);
}

Texture &Framebuffer::GetTexture()
{
	return depthBuffer_;
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &fbo_);
}
