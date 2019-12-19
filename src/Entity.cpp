#include "Entity.h"
#include "ChunkManager.h"

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

	for (int i = 0; i < _countof(axes); i++)
	{
		while (delta[axes[i]] != 0)
		{
			float sign = glm::sign(delta[axes[i]]);
			float edge = position_[axes[i]] + sign * size_[axes[i]] / 2.0f;

			float distToNext = Math::DistToBlock(edge, Math::Axis(axes[i]), sign == -1.0f);
			float offset = sign * glm::min(distToNext, glm::abs(delta[axes[i]]));

			glm::vec3 targetPos = position_;
			targetPos[axes[i]] += offset;

			std::vector<ChunkManager::BlockInfo> collisions = ChunkManager::Instance().GetBlocksInVolume(targetPos, size_);

			if (collisions.size() == 0)
			{
				position_ = targetPos;
				delta[axes[i]] -= offset;
			}
			else
			{
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

void Entity::OnCollision(std::pair<std::vector<ChunkManager::BlockInfo>, Math::Direction> collision)
{
	velocity_[collision.second / 2] = 0.0f;
}
