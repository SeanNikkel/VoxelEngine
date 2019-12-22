#include "ChunkManager.h"
#include "Math.h"
#include "WorldConstants.h"
#include "WindowManager.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtx/norm.hpp>

#include <unordered_set>

ChunkManager::ChunkManager() : shader_("shaders/shader.vert", "shaders/shader.frag"), texture_("resources/tileset.png", true, true, GL_REPEAT, GL_NEAREST)
{
	shader_.SetVar("tex", 0);
	shader_.SetVar("fogMin", World::renderDistance * 0.25f);
	shader_.SetVar("fogMax", World::renderDistance * 1.5f);
	shader_.SetVar("fogColor", WindowManager::Instance().GetClearColor());
}

ChunkManager::~ChunkManager()
{
	for (const auto &c : chunks_)
		delete c.second;
}

// Adds completed chunk to buffer, generates surrounding chunks
Chunk *ChunkManager::AddChunk(glm::ivec2 coord)
{
	Chunk *currentChunk = GetChunk(coord);
	if (currentChunk == nullptr)
	{
		currentChunk = new Chunk(coord);
		currentChunk->Generate(noise_);
		chunks_[coord] = currentChunk;
	}
	else if (currentChunk->MeshBuilt())
	{
		return currentChunk;
	}

	// Generate surrounding chunks and add
	for (unsigned i = 0; i < _countof(Math::surrounding); i++)
	{
		glm::ivec2 newCoord = coord + Math::surrounding[i];
		if (GetChunk(newCoord) == nullptr)
		{
			Chunk *newChunk = new Chunk(newCoord);
			newChunk->Generate(noise_);
			chunks_[newCoord] = newChunk;
		}
	}

	currentChunk->BuildMesh();
	return currentChunk;
}

bool ChunkManager::ChunkInRange(glm::vec3 playerPos, glm::vec3 chunkPos)
{
	glm::vec3 pos = chunkPos + glm::vec3(World::chunkSize, 0.0f, World::chunkSize) / 2.f;
	return glm::distance2(glm::vec2(pos.x, pos.z), glm::vec2(playerPos.x, playerPos.z)) <= World::renderDistance * World::renderDistance;
}

bool ChunkManager::HasBuiltNeighbor(glm::ivec2 coord, glm::ivec2 exclude)
{
	for (unsigned j = 0; j < _countof(Math::surrounding); j++)
	{
		glm::ivec2 currentCoord = coord + Math::surrounding[j];

		if (currentCoord == exclude)
			continue;

		Chunk *currentChunk = GetChunk(currentCoord);
		if (currentChunk != nullptr && currentChunk->MeshBuilt())
			return true;
	}
	return false;
}

bool ChunkManager::HasBuiltNeighbor(glm::ivec2 coord)
{
	return HasBuiltNeighbor(coord, coord);
}

void ChunkManager::UpdateChunks(glm::vec3 playerPos, float dt)
{
	unsigned loadedChunks = 0;

	// Create initial chunk
	glm::ivec2 playerChunkCoord = ToChunkPosition(glm::floor(playerPos));
	Chunk *playerChunk = GetChunk(playerChunkCoord);
	if (playerChunk == nullptr || !playerChunk->MeshBuilt())
	{
		loadedChunks++;
		AddChunk(playerChunkCoord);
	}

	ChunkContainer::iterator it = chunks_.begin();
	while (it != chunks_.end())
	{
		// Update the height timer
		it->second->UpdateHeightTimer(dt);

		// Build meshes of all chunks and add unmeshed ones surrounding
		if (loadedChunks < World::renderSpeed && !it->second->MeshBuilt() && ChunkInRange(playerPos, it->second->GetWorldPos()))
		{
			loadedChunks++;
			AddChunk(it->first);
		}

		// Unload if too far
		if (it->second->MeshBuilt() && !ChunkInRange(playerPos, it->second->GetWorldPos()))
		{
			// Delete surrounding chunks unconnected otherwise
			for (unsigned i = 0; i < _countof(Math::surrounding); i++)
			{
				glm::ivec2 newCoord = it->first + Math::surrounding[i];
				ChunkContainer::iterator chunk = chunks_.find(newCoord);

				if (chunk != chunks_.end() && !chunk->second->MeshBuilt() && !HasBuiltNeighbor(newCoord, it->first))
				{
					delete chunk->second;
					chunks_.erase(chunk);
				}
			}

			// Only remove mesh of chunk
			// TODO: add separate flag so mesh can stay until deletion
			if (HasBuiltNeighbor(it->first))
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

	//// Bad draw
	//SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), { 0, 0 });
	//for (int y = -20; y <= 20; y++)
	//{
	//	for (int x = -20; x <= 20; x++)
	//	{
	//		Chunk *chunk = GetChunk({ x, y });
	//		if (chunk == nullptr)
	//			std::cout << " ";
	//		else if (chunk->MeshBuilt())
	//			std::cout << ".";
	//		else
	//			std::cout << "#";

	//		std::cout << " ";
	//	}
	//	std::cout << std::endl;
	//}
}

void ChunkManager::DrawChunks(const glm::mat4 &cameraMatrix)
{
	shader_.Use();

	Math::Frustum camera = Math::CalculateFrustum(cameraMatrix);

	for (const auto &c : chunks_)
	{
		if (c.second->IsVisible(camera))
		{
			shader_.SetVar("MVP", glm::translate(cameraMatrix, c.second->GetRenderPos()));
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

	if (chunk->MeshBuilt())
		chunk->BuildMesh();

	for (int d = 0; d < Math::DIRECTION_COUNT; d++)
	{
		if (d == Math::DIRECTION_UP || d == Math::DIRECTION_DOWN)
			continue;

		Chunk *adjChunk = GetChunk(pos + static_cast<glm::ivec3>(Math::directionVectors[d]));
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
