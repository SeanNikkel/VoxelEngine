#include "InputManager.h"
#include "WindowManager.h"

InputManager::InputManager() : window_(nullptr), scroll_(0.0f), mousePos_(glm::vec2(0.0f, 0.0f)), prevMousePos_(glm::vec2(0.0f, 0.0f))
{
	WindowManager &windowManager = WindowManager::Instance();
	window_ = windowManager.GetWindow();

	mousePos_ = { windowManager.GetResolution().x / 2.f, windowManager.GetResolution().y / 2.f };
	prevMousePos_ = mousePos_;

	glfwSetCursorPosCallback(window_, &InputManager::MouseCallback);
	glfwSetScrollCallback(window_, &InputManager::ScrollCallback);
}

bool InputManager::GetKey(int key)
{
	if (key >= GLFW_MOUSE_BUTTON_1 && key <= GLFW_MOUSE_BUTTON_LAST)
		return glfwGetMouseButton(window_, key) == GLFW_PRESS;
	else
		return glfwGetKey(window_, key) == GLFW_PRESS;
}

glm::vec2 InputManager::GetDeltaMouse()
{
	return prevMousePos_ - mousePos_;
}

float InputManager::GetScroll()
{
	return scroll_;
}

void InputManager::Update()
{
	scroll_ = 0.0f;
	prevMousePos_ = mousePos_;

	glfwPollEvents();
}

void InputManager::MouseCallback(GLFWwindow *window, double xpos, double ypos)
{
	InputManager::Instance().mousePos_ = { xpos, ypos };
}

void InputManager::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	InputManager::Instance().scroll_ = (float)yoffset;
}
