#pragma once

#include "Camera.h"
#include "Entity.h"

// The player is an entity with input logic and a camera
class Player : public Entity
{
public:
	Player();

	// Handle input
	void Update(float dt);

	// Gets the player camera
	const Camera &GetCamera() const;

	// On collision override
	virtual void OnCollision(std::pair<std::vector<BlockInfo>, Math::Direction> collision) override;

private:
	Camera camera_;
	bool canJump_;
	bool noclip_;
};