#include "ChunkManager.h"
#include "Math.h"
#include "WorldConstants.h"
#include "WindowManager.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtx/norm.hpp>

#include <iostream>

ChunkManager::ChunkManager() : shader_("shaders/shader.vert", "shaders/shader.frag"), texture_("resources/tileset.png", true, true, GL_REPEAT, GL_NEAREST)
{
	// Default uniform variables
	shader_.SetVar("tex", 0);
	shader_.SetVar("fogAmount", 0.7f / World::renderDistance);
}

ChunkManager::~ChunkManager()
{
	for (const auto &c : chunks_)
		delete c.second;
}

Chunk *ChunkManager::AddChunk(glm::ivec2 coord)
{
	Chunk *currentChunk = GetChunk(coord);
	if (currentChunk == nullptr)
	{
		// Generate this chunk
		currentChunk = new Chunk(coord);
		currentChunk->Generate(noise_);
		chunks_[coord] = currentChunk;
	}
	else if (currentChunk->MeshBuilt())
	{
		// Chunk already exists
		return currentChunk;
	}

	// Generate surrounding chunks and add
	for (unsigned i = 0; i < std::size(Math::surrounding); i++)
	{
		glm::ivec2 newCoord = coord + Math::surrounding[i];
		if (GetChunk(newCoord) == nullptr)
		{
			Chunk *newChunk = new Chunk(newCoord);
			newChunk->Generate(noise_);
			chunks_[newCoord] = newChunk;
		}
	}

	// Build this chunk's mesh
	currentChunk->BuildMesh();
	return currentChunk;
}

bool ChunkManager::ChunkInRange(glm::vec3 playerPos, glm::vec3 chunkPos) const
{
	// Check if chunk is closer to player than render distance
	glm::vec3 pos = chunkPos + glm::vec3(World::chunkSize, 0.0f, World::chunkSize) / 2.f;
	float distanceSquared = glm::distance2(glm::vec2(pos.x, pos.z), glm::vec2(playerPos.x, playerPos.z));
	return distanceSquared <= World::renderDistance * World::renderDistance;
}

int ChunkManager::BuiltNeighborCount(glm::ivec2 coord, glm::ivec2 exclude) const
{
	unsigned count = 0;
	
	// Loop over surrounding chunks
	for (unsigned j = 0; j < std::size(Math::surrounding); j++)
	{
		glm::ivec2 currentCoord = coord + Math::surrounding[j];

		if (currentCoord == exclude)
			continue;

		// Count chunk if it's built
		const Chunk *currentChunk = GetChunk(currentCoord);
		if (currentChunk != nullptr && currentChunk->MeshBuilt())
			count++;
	}
	return count;
}

int ChunkManager::BuiltNeighborCount(glm::ivec2 coord) const
{
	return BuiltNeighborCount(coord, coord);
}

void ChunkManager::UpdateChunks(glm::vec3 playerPos, float dt)
{
	unsigned loadedChunks = 0;

	// Create initial chunks 
	glm::ivec2 playerChunkCoord = ToChunkPosition(glm::floor(playerPos));
	Chunk *playerChunk = GetChunk(playerChunkCoord);
	if (playerChunk == nullptr || !playerChunk->MeshBuilt())
	{
		// Create 3x3 cross shape of chunks on player
		loadedChunks++;
		AddChunk(playerChunkCoord);
		for (int i = 0; i < Math::DIRECTION_COUNT; i++)
		{
			if (i == Math::DIRECTION_UP || i == Math::DIRECTION_DOWN)
				continue;

			loadedChunks++;
			AddChunk(playerChunkCoord + glm::ivec2(Math::directionVectors[i].x, Math::directionVectors[i].z));
		}
	}

	// Update all chunks
	ChunkContainer::iterator it = chunks_.begin();
	while (it != chunks_.end())
	{
		// Update the height timer
		it->second->UpdateHeightTimer(dt);

		if (ChunkInRange(playerPos, it->second->GetWorldPos()))
		{
			// Build meshes of all chunks and add unmeshed ones surrounding
			if (loadedChunks < World::renderSpeed && !it->second->MeshBuilt() && BuiltNeighborCount(it->first) >= 3)
			{
				loadedChunks++;
				AddChunk(it->first);
			}

			// Move up if in range
			it->second->SetHeightTimerIncreasing(true);
		}
		// Move down if out of range
		else if (it->second->MeshBuilt())
		{
			it->second->SetHeightTimerIncreasing(false);
		}

		// Unload if all the way down
		if (it->second->HeightTimerHitZero())
		{
			// Delete surrounding chunks unconnected otherwise
			for (unsigned i = 0; i < std::size(Math::surrounding); i++)
			{
				glm::ivec2 newCoord = it->first + Math::surrounding[i];
				ChunkContainer::iterator chunk = chunks_.find(newCoord);

				if (chunk != chunks_.end() && !chunk->second->MeshBuilt() && BuiltNeighborCount(newCoord, it->first) == 0)
				{
					delete chunk->second;
					chunks_.erase(chunk);
				}
			}

			// Only remove mesh of chunk
			if (BuiltNeighborCount(it->first) > 0)
			{
				it->second->ClearMesh();
				++it;
			}
			else
			{
				delete it->second;
				ChunkContainer::iterator prev = it;
				++it;
				chunks_.erase(prev);
			}
		}
		else
			++it;
	}
}

void ChunkManager::DrawChunksLit(const Camera &camera, const std::vector<CascadeShaderInfo> &cascadeInfo)
{
	shader_.Use();
	texture_.Activate(GL_TEXTURE0);

	// Calculate camera frustum
	glm::mat4 cameraMatrix = camera.GetMatrix();

	// Camera uniforms
	shader_.SetVar("cameraPosition", camera.GetPosition());
	shader_.SetVar("nearPlane", camera.GetNearPlane());
	shader_.SetVar("farPlane", camera.GetFarPlane());

	// Upload cascade data
	for (size_t i = 0; i < cascadeInfo.size(); i++)
	{
		shader_.SetVar(("cascadeTransforms[" + std::to_string(i) + "]").c_str(), cascadeInfo[i].transform);
		shader_.SetVar(("cascadeDepths[" + std::to_string(i) + "]").c_str(), cascadeInfo[i].depth);
		shader_.SetVar(("cascades[" + std::to_string(i) + "]").c_str(), int(i + 1));
		cascadeInfo[i].tex->Activate(GLenum(GL_TEXTURE0 + i + 1));
	}
	DrawChunks(cameraMatrix, shader_);
}

void ChunkManager::DrawChunks(const glm::mat4 &cameraMatrix, const Shader &shader)
{
	shader.SetVar("cameraMatrix", cameraMatrix);

	Math::Frustum cameraFrustum = Math::CalculateFrustum(cameraMatrix);
	for (const auto &c : chunks_)
	{
		// Frustum culling
		if (c.second->IsVisible(cameraFrustum))
		{
			// Set shader/texture
			shader.SetVar("modelMatrix", glm::translate(glm::mat4(1.0f), c.second->GetRenderPos()));

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

	// Rebuild chunk mesh after modification
	if (chunk->MeshBuilt())
		chunk->BuildMesh();

	// Rebuild surrounding chunks if block was on edge
	for (std::size_t i = 0; i < std::size(Math::surrounding); i++)
	{
		Chunk *adjChunk = GetChunk(pos + glm::ivec3(Math::surrounding[i].x, 0.0f, Math::surrounding[i].y));
		if (adjChunk != nullptr && adjChunk != chunk && adjChunk->MeshBuilt())
			adjChunk->BuildMesh();
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

	// AABB bounds
	float xmin = pos.x - size.x / 2.f;
	float xmax = pos.x + size.x / 2.f;
	float ymin = pos.y - size.y / 2.f;
	float ymax = pos.y + size.y / 2.f;
	float zmin = pos.z - size.z / 2.f;
	float zmax = pos.z + size.z / 2.f;

	// AABB collision with blcok grid via indexing
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
	// Chunks arent const, cast is safe
	return const_cast<Chunk *>(static_cast<const ChunkManager &>(*this).GetChunk(pos));
}

const Chunk *ChunkManager::GetChunk(glm::ivec3 pos) const
{
	if (pos.y < 0 || pos.y >= World::chunkHeight)
		return nullptr;

	return GetChunk(ToChunkPosition(pos));
}

Chunk *ChunkManager::GetChunk(glm::ivec2 chunkCoord)
{
	// Chunks arent const, cast is safe
	return const_cast<Chunk *>(static_cast<const ChunkManager &>(*this).GetChunk(chunkCoord));
}

const Chunk *ChunkManager::GetChunk(glm::ivec2 chunkCoord) const
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
		pos.x -= World::chunkSize - 1;
	if (pos.z < 0)
		pos.z -= World::chunkSize - 1;

	return glm::ivec2(pos.x / (int)World::chunkSize, pos.z / (int)World::chunkSize);
}
