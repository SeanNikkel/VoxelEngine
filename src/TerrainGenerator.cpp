#include "TerrainGenerator.h"
#include "WorldConstants.h"

#include "glm/gtc/noise.hpp"
#include "glm/gtx/compatibility.hpp"

namespace Gen = World::Generation;

int TerrainGenerator::GetHeight(glm::vec2 pos)
{
	return static_cast<int>(GetPerlinHeight(pos));
}

float TerrainGenerator::GetPerlinHeight(glm::vec2 pos)
{
	// Check cache
	float cache = GetFromCache(pos);
	if (cache > 0.0f)
		return cache;

	// Calculate
	float height = ((glm::perlin(pos / static_cast<float>(Gen::heightScale)) + 1) / 2) * Gen::heightWeight * Gen::heightMaxHeight +
				   ((glm::perlin(pos / static_cast<float>(Gen::detailScale)) + 1) / 2) * Gen::detailWeight * Gen::detailMaxHeight;

	height += Gen::minHeight;

	AddToCache(pos, height);
	return height;
}

void TerrainGenerator::AddToCache(glm::vec2 pos, float height)
{
	if (cacheSize_ >= cacheCapacity)
		cacheSize_ = 0;

	cache_[cacheSize_] = { height, pos };
	cacheSize_++;
}

float TerrainGenerator::GetFromCache(glm::vec2 pos)
{
	// this is crazy
	for (unsigned i = cacheSize_; i--;)
	{
		if (cache_[i].pos == pos)
			return cache_[i].height;
	}
	return 0.0f;
}
