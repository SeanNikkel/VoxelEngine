#include "Shader.h"
#include "Texture.h"
#include "Camera.h"


#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ChunkManager.h"
#include "Player.h"
#include "InputManager.h"
#include "WindowManager.h"

int main()
{
	// Create game systems
	WindowManager &windowManager = WindowManager::Instance();
	windowManager.Maximize();
	ChunkManager &chunkManager = ChunkManager::Instance();
	InputManager &inputManager = InputManager::Instance();
	Player player;

	// Render loop
	while (!glfwWindowShouldClose(windowManager.GetWindow()))
	{
		// Delta time
		static float lastFrame = 0.0f;
		float currentFrame = (float)glfwGetTime();
		static float deltaTime;
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Update
		windowManager.Update(deltaTime);
		player.Update(deltaTime);
		chunkManager.UpdateChunks(player.GetCamera().GetPosition(), deltaTime);
		inputManager.Update();

		// Draw
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		chunkManager.DrawChunks(player.GetCamera().GetPerspectiveMatrix(windowManager.GetResolution()) * player.GetCamera().GetViewMatrix());
		glfwSwapBuffers(windowManager.GetWindow());
	}

	return 0;
}