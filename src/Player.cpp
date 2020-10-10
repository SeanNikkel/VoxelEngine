#include "Player.h"
#include "ChunkManager.h"
#include "InputManager.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <random>
#include <iostream>

Player::Player() : Entity(), camera_(GetPosition()), canJump_(false), noclip_(false)
{
	// Hardcoded starting coords
	Teleport(glm::vec3(520.5f, 102.0f, -320.5f));
}

void Player::Update(float dt)
{
	InputManager &input = InputManager::Instance();
	ChunkManager &chunk = ChunkManager::Instance();

	// Noclip
	if (input.GetKeyPressed(GLFW_KEY_F1))
		noclip_ = !noclip_;

	// Wireframe
	static bool wireframe = false;
	if (input.GetKeyPressed(GLFW_KEY_F2))
	{
		wireframe = !wireframe;

		if (wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Random teleport
	if (input.GetKeyPressed(GLFW_KEY_F3))
	{
		static std::default_random_engine rng((unsigned)glfwGetTime());
		static std::uniform_real_distribution<float> dist(-10000.0f, 10000.0f);

		Teleport(glm::vec3(dist(rng), 200, dist(rng)));
	}

	// Fast place
	static bool fastPlace = false;
	if (input.GetKeyPressed(GLFW_KEY_F4))
		fastPlace = !fastPlace;

	// Build
	bool placing = fastPlace ? input.GetKey(GLFW_MOUSE_BUTTON_RIGHT) : input.GetKeyPressed(GLFW_MOUSE_BUTTON_RIGHT);
	bool destroying = fastPlace ? input.GetKey(GLFW_MOUSE_BUTTON_LEFT) : input.GetKeyPressed(GLFW_MOUSE_BUTTON_LEFT);
	if (placing != destroying)
	{
		// Perform raycast
		ChunkManager::RaycastResult raycast = chunk.Raycast(camera_.GetPosition(), camera_.GetForward(), INFINITY);

		if (raycast.hit)
		{
			glm::ivec3 pos = raycast.block.first;

			if (destroying)
				chunk.SetBlock(pos, { Block::BLOCK_AIR }); // Set to air if destroying
			if (placing)
			{
				pos += glm::ivec3(Math::directionVectors[raycast.normal]);

				if (!Math::AABBCollision(GetPosition(), GetSize(), glm::vec3(pos) + glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 1.0f)))
					chunk.SetBlock(pos, { Block::BLOCK_DIRT }); // Set to dirt if placing and not overlapping player
			}
		}
	}

	// Look
	glm::vec2 deltaMouse = input.GetDeltaMouse() * 0.001f;
	camera_.SetYaw(camera_.GetYaw() + deltaMouse.x);
	camera_.SetPitch(camera_.GetPitch() + deltaMouse.y);

	if (!noclip_)
	{
		glm::vec3 dir = glm::vec3(0.0f);

		// Move input
		if (input.GetKey(GLFW_KEY_W))
			dir += camera_.GetForwardAligned();
		if (input.GetKey(GLFW_KEY_S))
			dir -= camera_.GetForwardAligned();
		if (input.GetKey(GLFW_KEY_A))
			dir -= camera_.GetRight();
		if (input.GetKey(GLFW_KEY_D))
			dir += camera_.GetRight();

		// Move
		if (dir != glm::vec3(0.0f))
			Move(glm::normalize(dir) * (input.GetKey(GLFW_KEY_LEFT_SHIFT) ? 10.f : 5.f) * dt);

		// Jump
		if (canJump_ && input.GetKey(GLFW_KEY_SPACE))
		{
			SetVelocity(glm::vec3(0.0f, 10.0f, 0.0f));
			canJump_ = false;
		}

		// Physics
		Entity::Update(dt);
	}
	else // noclip
	{
		SetVelocity(glm::vec3(0.0f, 0.0f, 0.0f));

		glm::vec3 dir = glm::vec3(0.0f);

		// Move
		if (input.GetKey(GLFW_KEY_W))
			dir += camera_.GetForward();
		if (input.GetKey(GLFW_KEY_S))
			dir -= camera_.GetForward();
		if (input.GetKey(GLFW_KEY_A))
			dir -= camera_.GetRight();
		if (input.GetKey(GLFW_KEY_D))
			dir += camera_.GetRight();
		if (input.GetKey(GLFW_KEY_SPACE))
			dir += camera_.GetUp();
		if (input.GetKey(GLFW_KEY_LEFT_CONTROL))
			dir -= camera_.GetUp();

		if (dir != glm::vec3(0.0f))
			Teleport(GetPosition() + glm::normalize(dir) * (input.GetKey(GLFW_KEY_LEFT_SHIFT) ? 100.f : 10.f) * dt);
	}

	// Update camera
	camera_.SetPosition(GetPosition() + glm::vec3(0.0f, GetSize().y * 0.4f, 0.0f));
}

const Camera &Player::GetCamera() const
{
	return camera_;
}

void Player::OnCollision(std::pair<std::vector<ChunkManager::BlockInfo>, Math::Direction> collision)
{
	if (collision.second == Math::DIRECTION_DOWN)
 		canJump_ = true;

	Entity::OnCollision(collision);
}
