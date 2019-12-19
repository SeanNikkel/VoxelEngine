#pragma once

#include "Camera.h"
#include "Entity.h"

class Player : public Entity
{
public:
	Player();
	void Update(float dt);

	const Camera &GetCamera() const;

	virtual void OnCollision(std::pair<std::vector<ChunkManager::BlockInfo>, Math::Direction> collision);

private:
	Camera camera_;
	bool canJump_;
	bool noclip_;
};