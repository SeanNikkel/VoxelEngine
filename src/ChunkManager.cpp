#include "ChunkManager.h"
#include "Math.h"
#include "WorldConstants.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <unordered_set>

ChunkManager::ChunkManager() : shader_("shaders/shader.vert", "shaders/shader.frag"), texture_("resources/tileset.png", true, true, GL_REPEAT, GL_NEAREST)
{
	shader_.SetVar("tex", 0);
}

ChunkManager::~ChunkManager()
{
	for (const auto &c : chunks_)
		delete c.second;
}

void ChunkManager::UpdateChunks(glm::vec3 playerPos)
{
	glm::ivec2 coord = ToChunkPosition(glm::floor(playerPos));
	unsigned loadedChunks = 0;
	std::unordered_set<glm::ivec2> undrawn;

	for (const auto &c : chunks_)
	{
		undrawn.insert(c.first);
	}

	for (int x = coord.x - World::renderDistance; x <= (int)(coord.x + World::renderDistance); x++)
	{
		for (int z = coord.y - World::renderDistance; z <= (int)(coord.y + World::renderDistance); z++)
		{
			glm::ivec2 chunkCoord = { x, z };
			Chunk *currentChunk = GetChunk(chunkCoord);

			if (currentChunk != nullptr)
			{
				undrawn.erase(chunkCoord);
			}
			else if (loadedChunks < World::renderSpeed)
			{
				Chunk *newChunk = new Chunk(chunkCoord.x, chunkCoord.y);
				newChunk->Generate(noise_);
				newChunk->BuildMesh(noise_);
				chunks_[chunkCoord] = newChunk;
				loadedChunks++;
			}
		}
	}

	for (const auto &u : undrawn)
	{
		auto undrawnChunk = chunks_.find(u);
		delete undrawnChunk->second;
		chunks_.erase(undrawnChunk);
	}
}

void ChunkManager::DrawChunks(const glm::mat4 &cameraMatrix)
{
	shader_.Use();

	Math::Frustum camera = Math::CalculateFrustum(cameraMatrix);

	for (const auto &c : chunks_)
	{
		if (c.second->IsVisible(camera))
		{
			shader_.SetVar("MVP", glm::translate(cameraMatrix, c.second->GetPos()));
			texture_.Activate(GL_TEXTURE0);

			c.second->Draw();
		}
	}
}

void ChunkManager::SetBlock(glm::ivec3 pos, const Block &block)
{
	Chunk *chunk = GetChunk(pos);
	
	if (chunk == nullptr || block == chunk->GetBlock(pos))
		return;

	chunk->SetBlock(pos, block);

	chunk->BuildMesh(noise_);

	for (int d = 0; d < Math::DIRECTION_COUNT; d++)
	{
		if (d == Math::DIRECTION_UP || d == Math::DIRECTION_DOWN)
			continue;

		Chunk *adjChunk = GetChunk(pos + static_cast<glm::ivec3>(Math::directionVectors[d]));
		if (adjChunk != nullptr && adjChunk != chunk)
			adjChunk->BuildMesh(noise_);
	}
}

const Block &ChunkManager::GetBlock(glm::ivec3 pos)
{
	Chunk *chunk = GetChunk(pos);

	if (chunk == nullptr)
	{
		static Block error = { Block::BLOCK_ERROR };
		return error;
	}

	return chunk->GetBlock(pos);
}

std::vector<ChunkManager::BlockInfo> ChunkManager::GetBlocksInVolume(glm::vec3 pos, glm::vec3 size)
{
	std::vector<BlockInfo> result;

	float xmin = pos.x - size.x / 2.f;
	float xmax = pos.x + size.x / 2.f;
	float ymin = pos.y - size.y / 2.f;
	float ymax = pos.y + size.y / 2.f;
	float zmin = pos.z - size.z / 2.f;
	float zmax = pos.z + size.z / 2.f;

	for (int x = (int)glm::floor(xmin); x <= (int)glm::floor(xmax - (glm::fract(xmax) == 0.0f ? 1.0f : 0.0f)); x++)
	{
		for (int y = (int)glm::floor(ymin); y <= (int)glm::floor(ymax - (glm::fract(ymax) == 0.0f ? 1.0f : 0.0f)); y++)
		{
			for (int z = (int)glm::floor(zmin); z <= (int)glm::floor(zmax - (glm::fract(zmax) == 0.0f ? 1.0f : 0.0f)); z++)
			{
				glm::ivec3 coord = { x, y, z };
				const Block &block = GetBlock(coord);
				if (block.type != Block::BLOCK_AIR)
				{
					result.push_back(BlockInfo(coord, block));
				}
			}
		}
	}

	return result;
}

ChunkManager::RaycastResult ChunkManager::Raycast(glm::vec3 pos, glm::vec3 dir, float length)
{
	dir = glm::normalize(dir);

	// Check for starting inside block
	{
		glm::ivec3 coord = glm::floor(pos);
		Block block = GetBlock(coord);
		if (block.type != Block::BLOCK_AIR)
		{
			RaycastResult result;
			result.hit = true;
			result.block = BlockInfo(coord, block);
			result.pos = pos;
			result.normal = Math::VectorToDir(-dir);
			return result;
		}
	}

	while (true)
	{
		// Calculate distances to block boundaries

		float dists[Math::AXIS_COUNT];
		float min = INFINITY;
		int axis;

		for (int i = 0; i < Math::AXIS_COUNT; i++)
		{
			dists[i] = Math::DistToBlock(pos, Math::Axis(i), dir);
			dists[i] *= glm::abs(1.f / dir[i]);
			if (dists[i] < min)
			{
				min = dists[i];
				axis = i;
			}
		}

		// Move along ray to next block boundary

		length -= min;

		if (length <= 0)
		{
			RaycastResult result;
			result.hit = false;
			return result;
		}

		pos += dir * min;

		// Check for block at block boundary

		glm::ivec3 blockCoord = glm::floor(pos);

		if (dir[axis] < 0.0f)
			blockCoord[axis]--;

		Block block = GetBlock(blockCoord);

		if (block.type != Block::BLOCK_AIR)
		{
			RaycastResult result;
			result.hit = true;
			result.block = BlockInfo(blockCoord, block);
			result.pos = pos;
			result.normal = Math::AxisToDir(Math::Axis(axis), !(dir[axis] < 0.0f));
			return result;
		}
	}
}

Chunk *ChunkManager::GetChunk(glm::ivec3 pos)
{
	if (pos.y < 0 || pos.y >= World::chunkHeight)
		return nullptr;

	return GetChunk(ToChunkPosition(pos));
}

Chunk *ChunkManager::GetChunk(glm::ivec2 chunkCoord)
{
	auto result = chunks_.find(chunkCoord);

	if (result == chunks_.end())
		return nullptr;

	return result->second;
}

glm::ivec2 ChunkManager::ToRelativePosition(glm::ivec3 pos) const
{
	return { Math::PositiveMod(pos.x, World::chunkSize), Math::PositiveMod(pos.z, World::chunkSize) };
}

glm::ivec2 ChunkManager::ToChunkPosition(glm::ivec3 pos) const
{
	if (pos.x < 0)
		pos.x -= 15;
	if (pos.z < 0)
		pos.z-= 15;

	return glm::ivec2(pos.x / (int)World::chunkSize, pos.z / (int)World::chunkSize);
}
