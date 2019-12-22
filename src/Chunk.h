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
	Chunk(glm::ivec2 pos);

	void Generate(TerrainGenerator &gen);

	void BuildMesh();
	void ClearMesh();
	bool MeshBuilt() const;

	void SetBlock(glm::ivec3 pos, const Block &block);
	const Block &GetBlock(glm::ivec3 pos) const;

	glm::ivec2 GetCoord() const;
	glm::vec3 GetWorldPos() const;
	glm::vec3 GetRenderPos() const; // affected by height

	void UpdateHeightTimer(float dt);
	void SetHeightTimerDirection(bool increasing);
	bool HeightTimerEmpty() const;

	bool IsVisible(const Math::Frustum &camera) const;

	void Draw();

private:
	glm::ivec2 position_;
	Mesh mesh_;
	float heightTimer_; // 0: down, 1: up
	bool heightTimerIncreasing_;
	
	// low to high: x, z, y
	std::array<Block, World::chunkSize * World::chunkSize * World::chunkHeight> blocks_ = {};

	glm::ivec3 WorldToLocal(glm::ivec3 pos) const;
	glm::ivec3 LocalToWorld(glm::ivec3 pos) const;
	bool OutOfBounds(glm::ivec3 pos) const;
	const Block &GetBlockLocal(glm::ivec3 pos) const;
	void SetBlockLocal(glm::ivec3 pos, const Block &block);
	bool CheckForBlock(glm::ivec3 pos) const;

};

