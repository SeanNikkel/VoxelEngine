#include "WindowManager.h"
#include "InputManager.h"

#include <glad/glad.h>
#include <string>

WindowManager::WindowManager() : window_(nullptr), resolution_(glm::ivec2(1920, 1080))
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
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
	SetClearColor({ 0.4f, 0.7f, 1.0f }); // sky

	// Z-buffer testing
	glEnable(GL_DEPTH_TEST);

	// Backface culling
	glEnable(GL_CULL_FACE);

	// Hide and capture cursor
	glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void WindowManager::Update(float dt)
{
	if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window_, true);

	// Framerate
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

	/*GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		std::cout << std::hex << err << std::endl;
	}*/
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
	glClearColor(color.r, color.g, color.b, 1.0f);
}

void WindowManager::Maximize()
{
	glfwMaximizeWindow(window_);
	glfwPollEvents();
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