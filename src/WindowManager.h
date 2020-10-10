#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Wrapper for GLFW windows
class WindowManager
{
public:
	// Singleton pattern
	static WindowManager &Instance()
	{
		static WindowManager instance;
		return instance;
	}

	WindowManager();
	void Update(float dt);

	// Getters
	GLFWwindow *GetWindow() const;
	glm::ivec2 GetResolution() const;
	glm::vec3 GetClearColor() const;

	// Setters
	void SetClearColor(glm::vec3 color);

	// Maximize this window
	void Maximize();

	~WindowManager();

private:
	GLFWwindow *window_;
	glm::ivec2 resolution_;
	glm::vec3 clearColor_;

	static void ResizeCallback(GLFWwindow *window, int width, int height); // window resize callback

public: // Remove functions for singleton
	WindowManager(WindowManager const &) = delete;
	void operator=(WindowManager const &) = delete;
};