#pragma once

#include <array>
#include <vector>

#include <glm/glm.hpp>

// Entry in cache
struct HeightCache
{
	float height; // height zero = disabled
	glm::vec2 pos;
};

// Defines method of terrain generation
class TerrainGenerator
{
public:
	// Get deterministic height value at coord
	int GetHeight(glm::vec2 pos);

	// Get deterministic tree points in area [start, end)
	std::vector<glm::ivec2> GenerateTreePoints(glm::ivec2 startCorner, glm::ivec2 endCorner);

	// y, x, z
	static const int tree[10][7][7];

private:
	static const unsigned cacheCapacity = 128;

	// Cache for height data
	std::array<HeightCache, cacheCapacity> cache_;
	unsigned cacheSize_ = 0;

	float GetNoiseHeight(glm::vec2 pos); // Get height of raw noise
	void AddToCache(glm::vec2 pos, float height); // Add height value to cache
	float GetFromCache(glm::vec2 pos); // Retrieve height value from cache
};