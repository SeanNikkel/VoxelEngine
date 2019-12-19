#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class WindowManager
{
public:
	static WindowManager &Instance()
	{
		static WindowManager instance;
		return instance;
	}

	WindowManager();

	void Update();

	GLFWwindow *GetWindow() const;
	glm::ivec2 GetResolution() const;

	void Maximize();

	~WindowManager();

private:
	GLFWwindow *window_;
	glm::ivec2 resolution_;

	static void ResizeCallback(GLFWwindow *window, int width, int height);

public:
	WindowManager(WindowManager const &) = delete;
	void operator=(WindowManager const &) = delete;
};