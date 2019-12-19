#pragma once

#include <array>

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

private:
	static const unsigned cacheCapacity = 64;

	std::array<HeightCache, cacheCapacity> cache_;
	unsigned cacheSize_ = 0;

	float GetPerlinHeight(glm::vec2 pos);
	void AddToCache(glm::vec2 pos, float height);
	float GetFromCache(glm::vec2 pos);
};