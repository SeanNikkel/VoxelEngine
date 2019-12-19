#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class InputManager
{
public:
	static InputManager &Instance()
	{
		static InputManager instance;
		return instance;
	}

	InputManager();

	bool GetKey(int key);
	glm::vec2 GetDeltaMouse();
	float GetScroll();

	void Update();

private:
	GLFWwindow *window_;
	float scroll_;
	glm::vec2 mousePos_;
	glm::vec2 prevMousePos_;

	static void MouseCallback(GLFWwindow *window, double xpos, double ypos);
	static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
public:
	InputManager(InputManager const &) = delete;
	void operator=(InputManager const &) = delete;
};