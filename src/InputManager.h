#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <unordered_map>

// Provide on pressed, on released, and is down key functions
class InputManager
{
public:
	// Singleton pattern
	static InputManager &Instance()
	{
		static InputManager instance;
		return instance;
	}

	InputManager();

	// Get key just pressed
	bool GetKeyPressed(int key);

	// Get key down
	bool GetKey(int key);

	// Get key just released
	bool GetKeyReleased(int key);

	// Get change in mouse movement this frame
	glm::vec2 GetDeltaMouse();

	// Get change in scroll this frame
	float GetScroll();

	// Reset variables and poll events
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

	void UpdateKeyStates(); // Set triggered/released to pressed/not

	// GLFW callbacks
	static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void MouseCallback(GLFWwindow *window, double xpos, double ypos);
	static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);

public: // Remove functions for singleton
	InputManager(InputManager const &) = delete;
	void operator=(InputManager const &) = delete;
};