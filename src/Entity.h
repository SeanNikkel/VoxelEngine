#pragma once

#include "ChunkManager.h"

#include <glm/glm.hpp>

class Entity
{
public:
	Entity();
	void Update(float dt);

	glm::vec3 GetPosition() const;
	glm::vec3 GetVelocity() const;
	glm::vec3 GetSize() const;

	void Move(glm::vec3 delta);
	void Teleport(glm::vec3 destination);

	void SetVelocity(glm::vec3 vel);

	virtual void OnCollision(std::pair<std::vector<ChunkManager::BlockInfo>, Math::Direction> collision);

private:
	glm::vec3 position_;
	glm::vec3 velocity_;
	glm::vec3 size_;
};