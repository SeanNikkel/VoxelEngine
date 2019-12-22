#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <unordered_map>

class InputManager
{
public:
	static InputManager &Instance()
	{
		static InputManager instance;
		return instance;
	}

	InputManager();

	bool GetKeyPressed(int key);
	bool GetKey(int key);
	bool GetKeyReleased(int key);
	glm::vec2 GetDeltaMouse();
	float GetScroll();

	void Update();

private:
	enum KeyState
	{
		KEY_UP,
		KEY_PRESSED,
		KEY_HELD,
		KEY_RELEASED,
	};

	GLFWwindow *window_;
	float scroll_;
	glm::vec2 mousePos_;
	glm::vec2 prevMousePos_;
	std::unordered_map<int, KeyState> keyStates_;

	void UpdateKeyStates();

	static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void MouseCallback(GLFWwindow *window, double xpos, double ypos);
	static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
public:
	InputManager(InputManager const &) = delete;
	void operator=(InputManager const &) = delete;
};