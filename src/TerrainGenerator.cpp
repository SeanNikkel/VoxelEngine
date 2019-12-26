#include "TerrainGenerator.h"
#include "WorldConstants.h"

#include <glm/gtc/noise.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/hash.hpp>

#include <random>

namespace Gen = World::Generation;

const int TerrainGenerator::tree[10][7][7] = {
	{
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 4, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
	},
	{
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 4, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
	},
	{
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 4, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
	},
	{
		{ 0, 0, 0, 5, 0, 0, 0 },
		{ 0, 0, 5, 5, 5, 0, 0 },
		{ 0, 5, 5, 5, 5, 5, 0 },
		{ 5, 5, 5, 4, 5, 5, 5 },
		{ 0, 5, 5, 5, 5, 5, 0 },
		{ 0, 0, 5, 5, 5, 0, 0 },
		{ 0, 0, 0, 5, 0, 0, 0 },
	},
	{
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 5, 5, 5, 0, 0 },
		{ 0, 0, 5, 4, 5, 0, 0 },
		{ 0, 0, 5, 5, 5, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
	},
	{
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 5, 5, 5, 0, 0 },
		{ 0, 5, 5, 5, 5, 5, 0 },
		{ 0, 5, 5, 4, 5, 5, 0 },
		{ 0, 5, 5, 5, 5, 5, 0 },
		{ 0, 0, 5, 5, 5, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
	},
	{
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 5, 0, 0, 0 },
		{ 0, 0, 5, 4, 5, 0, 0 },
		{ 0, 0, 0, 5, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
	},
	{
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 5, 0, 0, 0 },
		{ 0, 0, 5, 5, 5, 0, 0 },
		{ 0, 5, 5, 4, 5, 5, 0 },
		{ 0, 0, 5, 5, 5, 0, 0 },
		{ 0, 0, 0, 5, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
	},
	{
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 5, 0, 0, 0 },
		{ 0, 0, 5, 5, 5, 0, 0 },
		{ 0, 0, 0, 5, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
	},
	{
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 5, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
		{ 0, 0, 0, 0, 0, 0, 0 },
	},
};

int TerrainGenerator::GetHeight(glm::vec2 pos)
{
	glm::vec2 scaled = pos / Gen::terrainInterpGrid;
	glm::vec2 min = glm::floor(scaled) * Gen::terrainInterpGrid;
	glm::vec2 max = glm::ceil(scaled) * Gen::terrainInterpGrid;

	if (min == max)
		return static_cast<int>(GetPerlinHeight(pos));

	// interp x and y
	if (min.x != max.x && min.y != max.y)
	{
		float bl = GetPerlinHeight(min);
		float tr = GetPerlinHeight(max);
		float tl = GetPerlinHeight({ min.x, max.y });
		float br = GetPerlinHeight({ max.x, min.y });

		float tx = (pos.x - min.x) / (max.x - min.x);
		float ty = (pos.y - min.y) / (max.y - min.y);

		float ml = glm::lerp(bl, tl, ty);
		float mr = glm::lerp(br, tr, ty);

		return static_cast<int>(glm::lerp(ml, mr, tx));
	}
	
	// 1D interp
	float minH = GetPerlinHeight(min);
	float maxH = GetPerlinHeight(max);

	if (min.x == max.x)
		return static_cast<int>(glm::lerp(minH, maxH, (pos.y - min.y) / (max.y - min.y)));
	else
		return static_cast<int>(glm::lerp(minH, maxH, (pos.x - min.x) / (max.x - min.x)));
}

std::vector<glm::ivec2> TerrainGenerator::GenerateTreePoints(glm::ivec2 startCorner, glm::ivec2 endCorner)
{
	std::vector<glm::ivec2> points;
	std::hash<glm::ivec2> hash;
	std::uniform_real_distribution<float> dist(0.0f, 1.0f);

	unsigned attempts = 0;

	for (int z = startCorner.y; z < endCorner.y; z++)
	{
		for (int x = startCorner.x; x < endCorner.x; x++)
		{
			glm::ivec2 pos = { x, z };
			std::default_random_engine rng(hash(pos));

			if (dist(rng) <= Gen::treeDensity)
			{
				points.push_back(pos);
			}
		}
	}

	return points;
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

	height *= (glm::clamp((glm::perlin(pos / static_cast<float>(Gen::landScale)) + Gen::landMountainBias * 2.0f) * Gen::landTransitionSharpness, -1.0f + Gen::landMinMult * 2.0f, 1.0f) + 1.0f) / 2.0f;

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
