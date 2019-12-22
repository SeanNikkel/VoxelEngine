#include "InputManager.h"
#include "WindowManager.h"

InputManager::InputManager() : window_(nullptr), scroll_(0.0f), mousePos_(glm::vec2(0.0f, 0.0f)), prevMousePos_(glm::vec2(0.0f, 0.0f))
{
	WindowManager &windowManager = WindowManager::Instance();
	window_ = windowManager.GetWindow();

	double xpos, ypos;
	glfwGetCursorPos(window_, &xpos, &ypos);
	mousePos_ = { xpos, ypos };
	prevMousePos_ = mousePos_;

	glfwSetKeyCallback(window_, &InputManager::KeyCallback);
	glfwSetCursorPosCallback(window_, &InputManager::MouseCallback);
	glfwSetMouseButtonCallback(window_, &InputManager::MouseButtonCallback);
	glfwSetScrollCallback(window_, &InputManager::ScrollCallback);
}

bool InputManager::GetKeyPressed(int key)
{
	return keyStates_[key] == KEY_PRESSED;
}

bool InputManager::GetKey(int key)
{
	const KeyState &state = keyStates_[key];
	return state == KEY_PRESSED || state == KEY_HELD;
}

bool InputManager::GetKeyReleased(int key)
{
	return keyStates_[key] == KEY_RELEASED;
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

	UpdateKeyStates();
	glfwPollEvents();
}

void InputManager::UpdateKeyStates()
{
	for (auto it = keyStates_.begin(); it != keyStates_.end(); it++)
	{
		if (it->second == KEY_PRESSED)
			it->second = KEY_HELD;

		if (it->second == KEY_RELEASED)
			it->second = KEY_UP;
	}
}

void InputManager::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	InputManager &input = InputManager::Instance();

	switch (action)
	{
	case GLFW_PRESS:
		input.keyStates_[key] = KEY_PRESSED;
		break;
	case GLFW_RELEASE:
		input.keyStates_[key] = KEY_RELEASED;
		break;
	}
}

void InputManager::MouseCallback(GLFWwindow *window, double xpos, double ypos)
{
	InputManager::Instance().mousePos_ = { xpos, ypos };
}

void InputManager::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
	InputManager::Instance().KeyCallback(window, button, 0, action, mods);
}

void InputManager::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset)
{
	InputManager::Instance().scroll_ = (float)yoffset;
}
