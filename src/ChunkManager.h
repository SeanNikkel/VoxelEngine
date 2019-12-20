#pragma once

#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include "Block.h"
#include "Chunk.h"
#include "Shader.h"
#include "Texture.h"

class ChunkManager
{
public:
	static ChunkManager &Instance()
	{
		static ChunkManager instance;
		return instance;
	}

	typedef std::pair<glm::ivec3, Block> BlockInfo;

	struct RaycastResult
	{
		bool hit;
		BlockInfo block;
		glm::vec3 pos;
		Math::Direction normal;
	};

	void UpdateChunks(glm::vec3 playerPos);
	void DrawChunks(const glm::mat4 &cameraMatrix);

	void SetBlock(glm::ivec3 pos, const Block &block);
	const Block &GetBlock(glm::ivec3 pos);

	std::vector<BlockInfo> GetBlocksInVolume(glm::vec3 pos, glm::vec3 size);
	RaycastResult Raycast(glm::vec3 pos, glm::vec3 dir, float length = INFINITY);

private:
	typedef std::unordered_map<glm::ivec2, Chunk *> ChunkContainer;

	Shader shader_;
	Texture texture_;
	ChunkContainer chunks_;
	TerrainGenerator noise_;

	ChunkManager();
	~ChunkManager();
	Chunk *AddChunk(glm::ivec2 coord);
	bool ChunkInRange(glm::vec3 playerPos, glm::vec3 chunkPos);
	Chunk *GetChunk(glm::ivec3 pos);
	Chunk *GetChunk(glm::ivec2 chunkCoord);
	glm::ivec2 ToRelativePosition(glm::ivec3 pos) const;
	glm::ivec2 ToChunkPosition(glm::ivec3 pos) const;

public:
	ChunkManager(ChunkManager const &) = delete;
	void operator=(ChunkManager const &) = delete;
};