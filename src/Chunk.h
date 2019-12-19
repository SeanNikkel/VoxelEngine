#pragma once

#include <array>

#include <glm/glm.hpp>

#include "Mesh.h"
#include "WorldConstants.h"
#include "Block.h"
#include "TerrainGenerator.h"

class Chunk
{
public:
	Chunk(int x, int y);

	void Generate(TerrainGenerator &gen);

	void BuildMesh(TerrainGenerator &gen);

	void SetBlock(glm::ivec3 pos, const Block &block);
	const Block &GetBlock(glm::ivec3 pos) const;

	glm::ivec2 GetCoord() const;
	glm::vec3 GetPos() const;

	bool IsVisible(const Math::Frustum &camera) const;

	void Draw();

private:
	glm::ivec2 position_;
	Mesh mesh_;
	
	// low to high: x, z, y
	std::array<Block, World::chunkSize * World::chunkSize * World::chunkHeight> blocks_ = {};

	glm::ivec2 GetWorldPos() const;
	glm::ivec3 WorldToLocal(glm::ivec3 pos) const;
	glm::ivec3 LocalToWorld(glm::ivec3 pos) const;
	bool OutOfBounds(glm::ivec3 pos) const;
	const Block &GetBlockLocal(glm::ivec3 pos) const;
	void SetBlockLocal(glm::ivec3 pos, const Block &block);
	bool CheckForBlock(glm::ivec3 pos, TerrainGenerator &gen) const;

};

