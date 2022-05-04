#pragma once

#include <vector>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include "Block.h"
#include "Chunk.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "CascadedShadowMap.h"

// Loads, unloads, and draws chunks
class ChunkManager
{
public:
	// Singleton pattern
	static ChunkManager &Instance()
	{
		static ChunkManager instance;
		return instance;
	}

	typedef std::pair<glm::ivec3, Block> BlockInfo;

	// Raycast data
	struct RaycastResult
	{
		bool hit;
		BlockInfo block;
		glm::vec3 pos;
		Math::Direction normal;
	};

	// Load and unload chunks for this frame
	void UpdateChunks(glm::vec3 playerPos, float dt);

	// Draw all chunks with lighting calculations
	void DrawChunksLit(const Camera &camera, const std::vector<CascadeShaderInfo> &cascadeInfo);

	// Draw chunks with given shader and camera
	void DrawChunks(const glm::mat4 &cameraMatrix, const Shader &shader);

	// World block getters/setters
	void SetBlock(glm::ivec3 pos, const Block &block);
	const Block &GetBlock(glm::ivec3 pos);

	// Utility functions
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
	Chunk *AddChunk(glm::ivec2 coord); // adds completed chunk to buffer, generates surrounding chunks
	bool ChunkInRange(glm::vec3 playerPos, glm::vec3 chunkPos) const; // if chunk should stay loaded
	int BuiltNeighborCount(glm::ivec2 coord) const; // how many surrounding chunks' meshes are built
	int BuiltNeighborCount(glm::ivec2 coord, glm::ivec2 exclude) const;
	Chunk *GetChunk(glm::ivec3 pos); // Chunk getters
	const Chunk *GetChunk(glm::ivec3 pos) const;
	Chunk *GetChunk(glm::ivec2 chunkCoord);
	const Chunk *GetChunk(glm::ivec2 chunkCoord) const;
	glm::ivec2 ToRelativePosition(glm::ivec3 pos) const; // Convert block coord to local coord
	glm::ivec2 ToChunkPosition(glm::ivec3 pos) const; // Convert world coord to chunk coord

public: // Remove functions for singleton
	ChunkManager(ChunkManager const &) = delete;
	void operator=(ChunkManager const &) = delete;
};