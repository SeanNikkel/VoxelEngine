#include "WindowManager.h"
#include "InputManager.h"
#include "../shaders/Shared.h"

#include <glad/glad.h>
#include <string>

WindowManager::WindowManager() : window_(nullptr), resolution_(glm::ivec2(1280, 720))
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window
	window_ = glfwCreateWindow(resolution_.x, resolution_.y, "Voxel", nullptr, nullptr);

	if (window_ == nullptr)
	{
		assert(!"Failed to create GLFW window");
		glfwTerminate();
		exit(-1);
	}
	glfwMakeContextCurrent(window_);
	glfwSetFramebufferSizeCallback(window_, ResizeCallback);

	// Load GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		assert(!"Failed to initialize GLAD");
		glfwTerminate();
		exit(-1);
	}

	// Tell OpenGL the size of the rendering window
	glViewport(0, 0, resolution_.x, resolution_.y);

	// Set background color
	SetClearColor({ FOG_COLOR }); // sky

	// Z-buffer testing
	glEnable(GL_DEPTH_TEST);

	// Backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// Hide and capture cursor
	glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void WindowManager::Update(float dt)
{
	// Escape to exit
	if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window_, true);

	// Framerate display
	static unsigned frameCount = 0;
	static float frameTimer = 0.0f;
	frameCount++;
	frameTimer += dt;
	if (frameTimer >= 1.0f)
	{
		glfwSetWindowTitle(window_, ("Voxel - " + std::to_string(frameCount) + " FPS").c_str());
		frameCount = 0;
		frameTimer = 0.0f;
	}
}

GLFWwindow *WindowManager::GetWindow() const
{
	return window_;
}

glm::ivec2 WindowManager::GetResolution() const
{
	return resolution_;
}

glm::vec3 WindowManager::GetClearColor() const
{
	return clearColor_;
}

void WindowManager::SetClearColor(glm::vec3 color)
{
	clearColor_ = color;
	glClearColor(clearColor_.x, clearColor_.y, clearColor_.z, 1.0f);
}

void WindowManager::Maximize()
{
	glfwMaximizeWindow(window_);
	glfwPollEvents(); // prevents jolt
}

void WindowManager::SetFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, resolution_.x, resolution_.y);
	glClearColor(clearColor_.x, clearColor_.y, clearColor_.z, 1.0f);
}

WindowManager::~WindowManager()
{
	glfwTerminate();
}

void WindowManager::ResizeCallback(GLFWwindow *window, int width, int height)
{
	WindowManager::Instance().resolution_ = { width, height };
	glViewport(0, 0, width, height);
}