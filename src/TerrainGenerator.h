#pragma once

#include <array>
#include <vector>

#include <glm/glm.hpp>

struct HeightCache
{
	float height; // height zero = disabled
	glm::vec2 pos;
};

class TerrainGenerator
{
public:
	int GetHeight(glm::vec2 pos);

	// [start, end)
	std::vector<glm::ivec2> GenerateTreePoints(glm::ivec2 startCorner, glm::ivec2 endCorner);

	// This should be deserialized
	// y, x, z
	static const int tree[10][7][7];

private:
	static const unsigned cacheCapacity = 128;


	std::array<HeightCache, cacheCapacity> cache_;
	unsigned cacheSize_ = 0;

	float GetNoiseHeight(glm::vec2 pos);
	void AddToCache(glm::vec2 pos, float height);
	float GetFromCache(glm::vec2 pos);
};