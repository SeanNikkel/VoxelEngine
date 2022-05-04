#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "../shaders/Shared.h"
#include "Skybox.h"
#include "Crosshair.h"

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
#include "CascadedShadowMap.h"

#ifdef _DEBUG
int main()
#else
int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
#endif
{
	// Create game systems
	WindowManager &windowManager = WindowManager::Instance();
	windowManager.Maximize();
	ChunkManager &chunkManager = ChunkManager::Instance();
	InputManager &inputManager = InputManager::Instance();
	Player player;
	CascadedShadowMap shadows({ { 2048, 0.025f }, { 2048, 0.125f }, { 2048, 1.0f } });
	Skybox skybox;
	Crosshair crosshair;

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

		const Camera &cam = player.GetCamera();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);				// Clear
		shadows.Render(cam.GetMatrix());								// Render all the chunks to cascaded shadow maps
		skybox.Render(cam.GetViewMatrix(), cam.GetProjectionMatrix());	// Render the skybox
		chunkManager.DrawChunksLit(cam, shadows.GetShaderInfo());	// Render all the chunks to the screen
		if (!player.GetNoclip())
			crosshair.Render(glm::vec2(1.f, cam.GetAspect()) / 400.f);	// Render the crosshair if walking
		glfwSwapBuffers(windowManager.GetWindow());						// Present frame to screen
	}

	return 0;
}