#include "WindowManager.h"
#include "InputManager.h"

#include <glad/glad.h>

WindowManager::WindowManager() : window_(nullptr), resolution_(glm::ivec2(1920, 1080))
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window
	window_ = glfwCreateWindow(resolution_.x, resolution_.y, "Voxel", nullptr, nullptr);

	//const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	//screenWidth = mode->width;
	//screenHeight = mode->height;
	//GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "Voxel", glfwGetPrimaryMonitor(), nullptr);

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
	//glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
	glClearColor(0.4f, 0.7f, 1.0f, 1.0f); // sky

	// Z-buffer testing
	glEnable(GL_DEPTH_TEST);

	// Backface culling
	glEnable(GL_CULL_FACE);

	// Hide and capture cursor
	glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void WindowManager::Update()
{
	if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window_, true);

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		//std::cout << std::hex << err << std::endl;
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