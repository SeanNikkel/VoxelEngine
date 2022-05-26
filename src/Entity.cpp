#include "Entity.h"
#include "ChunkManager.h"
#include "WorldConstants.h"

Entity::Entity() : position_(glm::vec3(0.0f, 0.0f, 0.0f)), velocity_(glm::vec3(0.0f, 0.0f, 0.0f)), size_(glm::vec3(0.8f, 1.8f, 0.8f))
{
}

void Entity::Update(float dt)
{
	velocity_ -= glm::vec3(0.0f, World::gravity, 0.0f) * dt;

	Move(velocity_ * dt);
}

glm::vec3 Entity::GetPosition() const
{
	return position_;
}

glm::vec3 Entity::GetVelocity() const
{
	return velocity_;
}

glm::vec3 Entity::GetSize() const
{
	return size_;
}

void Entity::Move(glm::vec3 delta)
{
	// Check x, then z, then y
	int axes[] = { Math::AXIS_X, Math::AXIS_Z, Math::AXIS_Y };

	// For each axis
	for (std::size_t i = 0; i < std::size(axes); i++)
	{
		while (delta[axes[i]] != 0)
		{
			float sign = glm::sign(delta[axes[i]]); // direction
			float edge = position_[axes[i]] + sign * size_[axes[i]] / 2.0f; // entity edge offset

			float distToNext = Math::DistToBlock(edge, Math::Axis(axes[i]), sign == -1.0f); // distance to next block
			float offset = sign * glm::min(distToNext, glm::abs(delta[axes[i]])); // distance to move

			glm::vec3 targetPos = position_;
			targetPos[axes[i]] += offset;

			// Check for collisions in the target location
			std::vector<BlockInfo> collisions = ChunkManager::Instance().GetBlocksInVolume(targetPos, size_);

			if (collisions.size() == 0)
			{
				// No collision, take axis movement
				position_ = targetPos;
				delta[axes[i]] -= offset;
			}
			else
			{
				// Collision, stop moving on axis
				OnCollision(std::make_pair(collisions, Math::AxisToDir(Math::Axis(axes[i]), sign == -1.f)));
				break;
			}
		}
	}
}

void Entity::Teleport(glm::vec3 destination)
{
	position_ = destination;
}

void Entity::SetVelocity(glm::vec3 vel)
{
	velocity_ = vel;
}

void Entity::OnCollision(std::pair<std::vector<BlockInfo>, Math::Direction> collision)
{
	velocity_[collision.second / 2] = 0.0f;
}
